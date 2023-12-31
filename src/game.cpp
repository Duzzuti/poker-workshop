#include "game.h"

#include <algorithm>

#include "check_player/check_player.h"
#include "deck.h"
#include "hand_strengths.h"
#include "human_player/human_player.h"
#include "logger.h"
#include "rand_player/rand_player.h"

void Game::run() {
    // config players
    // init players
    this->players[0] = std::move(std::make_unique<CheckPlayer>(1));
    this->players[1] = std::move(std::make_unique<RandPlayer>(2));
    this->players[2] = std::move(std::make_unique<CheckPlayer>(3));
    this->players[3] = std::move(std::make_unique<RandPlayer>(4));
    this->players[4] = std::move(std::make_unique<RandPlayer>(5));

    this->data.numPlayers = this->m_config.numPlayers;

    // run for the number of games specified in the config
    for (u_int64_t game = 0; game < this->m_config.numRounds; game++) {
        // ONE GAME
        // shuffle players
        PLOG_DEBUG << "Starting game " << game;
        this->initPlayerOrder();
        this->data.gameData.numNonOutPlayers = this->m_config.numPlayers;
        std::memset(this->data.gameData.playerOut, 0, sizeof(this->data.gameData.playerOut));  // reset player out
        for (u_int8_t i = 0; i < this->m_config.numPlayers; i++) this->data.gameData.playerChips[i] = this->m_config.startingChips;
        int32_t round = -1;

        while (this->data.gameData.numNonOutPlayers > 1) {
            // ONE ROUND
            round++;
            this->deck = Deck();
            this->data.roundData.result = OutEnum::ROUND_CONTINUE;
            this->data.roundData.numNonFoldedPlayers = this->data.gameData.numNonOutPlayers;

            PLOG_DEBUG << "Starting round " << round;
            this->startRound(round == 0);
            // PREFLOP
            this->preflop();
            // FLOP
            this->flop();
            // TURN
            this->turn();
            // RIVER
            this->river();

            if (this->data.roundData.result == OutEnum::GAME_WON) {
                PLOG_INFO << "Game " << game << " ended in round " << round;
                break;
            } else if (this->data.roundData.result == OutEnum::ROUND_WON) {
                PLOG_DEBUG << "Pot won, starting new round";
                continue;
            }

            // SHOWDOWN
            PLOG_DEBUG << "SHOWDOWN!!! Community cards: " << this->data.roundData.communityCards[0].toString() << " " << this->data.roundData.communityCards[1].toString() << " "
                       << this->data.roundData.communityCards[2].toString() << " " << this->data.roundData.communityCards[3].toString() << " " << this->data.roundData.communityCards[4].toString();
            // get hand strength for each player
            HandStrengths handStrengths[data.numPlayers];
            HandStrengths::getHandStrengths(this->players, this->data, handStrengths);
            // get winner

            HandStrengths strongestHand = HandStrengths(HandKinds::NO_HAND, 0);
            u_int8_t winners[this->data.numPlayers];
            u_int8_t numWinners = 0;

            for (u_int8_t i = 0; i < this->data.numPlayers; i++) {
                if (this->data.gameData.playerOut[i] || this->data.roundData.playerFolded[i]) continue;
                PLOG_DEBUG << "Player " << +i << " has hand " << this->players[i]->getHand().first.toString() << " " << this->players[i]->getHand().second.toString() << " and hand strength "
                           << handStrengths[i].handkind << " " << handStrengths[i].rankStrength;
                if (handStrengths[i] > strongestHand) {
                    strongestHand = handStrengths[i];
                    numWinners = 1;
                    winners[0] = i;
                } else if (handStrengths[i] == strongestHand) {
                    winners[numWinners++] = i;
                }
            }

            // distribute pot
            u_int64_t potPerWinner = this->data.roundData.pot / numWinners;
            for (u_int8_t i = 0; i < numWinners; i++) {
                this->data.gameData.playerChips[winners[i]] += potPerWinner;
            }
        }
    }
}

std::string Game::getPlayerInfo() const noexcept { return "Player " + this->players[this->data.betRoundData.playerPos]->getName() + "[" + std::to_string(this->data.getChips()) + "]"; }

void Game::initPlayerOrder() noexcept {
    // shuffle deck
    std::random_shuffle(&this->players[0], &this->players[this->m_config.numPlayers]);
    PLOG_INFO << "Shuffled players, new order:";
    for (u_int8_t i = 0; i < this->m_config.numPlayers; i++) {
        this->players[i]->setPlayerPosNum(i);
        PLOG_INFO << this->players[i]->getName();
    }
}

OutEnum Game::setBlinds() noexcept {
    // blinds
    if (this->data.gameData.numNonOutPlayers == 2) {
        // heads up rule
        // adjust button if the big blind would be the same player again
        if (this->data.roundData.bigBlindPos != this->data.roundData.dealerPos) this->data.roundData.dealerPos = this->data.roundData.bigBlindPos;
        this->data.betRoundData.playerPos = this->data.roundData.dealerPos;
    }
    OutEnum res = OutEnum::ROUND_CONTINUE;
    PLOG_DEBUG << this->getPlayerInfo() << " bets small blind " << this->data.roundData.smallBlind;
    this->data.roundData.smallBlindPos = this->data.betRoundData.playerPos;
    while (!this->bet(this->data.roundData.smallBlind, true)) {
        res = this->playerOut();
        if (res != OutEnum::ROUND_CONTINUE) return res;
        PLOG_DEBUG << this->getPlayerInfo() << " bets small blind " << this->data.roundData.smallBlind;
        this->data.roundData.smallBlindPos = this->data.betRoundData.playerPos;
    }
    PLOG_DEBUG << this->getPlayerInfo() << " bets big blind " << this->data.roundData.bigBlind;
    this->data.roundData.bigBlindPos = this->data.betRoundData.playerPos;
    while (!this->bet(this->data.roundData.bigBlind, true)) {
        res = this->playerOut();
        if (res != OutEnum::ROUND_CONTINUE) return res;
        PLOG_DEBUG << this->getPlayerInfo() << " bets big blind " << this->data.roundData.bigBlind;
        this->data.roundData.bigBlindPos = this->data.betRoundData.playerPos;
    }
    return OutEnum::ROUND_CONTINUE;
}

void Game::setupBetRound() noexcept {
    this->data.betRoundData.playerPos = this->data.roundData.dealerPos;
    this->data.nextPlayer();
    std::memset(this->data.betRoundData.playerBets, 0, sizeof(this->data.betRoundData.playerBets));  // reset player bets
    this->data.betRoundData.currentBet = 0;
}

OutEnum Game::startRound(const bool firstRound) {
    this->deck.shuffle();
    u_int8_t dealerPos;
    if (!firstRound) dealerPos = this->data.roundData.dealerPos;
    this->data.selectDealer(firstRound);

    this->data.roundData.addBlind = this->m_config.addBlindPerDealer0;
    if (firstRound)
        this->data.roundData.smallBlind = this->m_config.smallBlind;
    else if (this->data.roundData.dealerPos < dealerPos)  // if the dealer is at position 0 again (or skipped 0), add the addBlind amount
        this->data.roundData.smallBlind += this->m_config.addBlindPerDealer0;
    this->data.roundData.bigBlind = this->data.roundData.smallBlind * 2;
    this->data.roundData.pot = 0;
    std::memset(this->data.roundData.playerFolded, 0, sizeof(this->data.roundData.playerFolded));  // reset player folded
    this->setupBetRound();

    // deal cards
    for (u_int8_t i = 0; i < this->m_config.numPlayers; i++) {
        if (this->data.gameData.playerOut[i]) continue;
        this->players[i]->setHand(this->deck.draw(), this->deck.draw());
    }

    return this->setBlinds();
}

OutEnum Game::betRound() {
    short firstChecker = -1;  // position of the first player that checked
    this->data.betRoundData.minimumRaise = this->data.roundData.bigBlind;
    // this loop will run until all players have either folded, checked or called
    // we can only exit if it is a players turn and he is in the game, has the same bet as the current bet and all players have checked if the bet is 0
    // we need to consider the case where every player folds except one, then the last player wins the pot
    while (this->betRoundContinue(firstChecker)) {
        if (!this->currentPlayerActive()) {
            // player is out of the game or folded, skip turn
            this->data.nextPlayer();
            continue;
        } else if (this->currentPlayerCanOnlyRaiseOrCall()) {
            // player can only raise or call
            OutEnum turnRes = this->playerTurnOnlyRaise();
            if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
        } else {
            OutEnum turnRes = this->playerTurn(firstChecker);
            if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
        }
    }

    PLOG_DEBUG << "Bet round finished with bet " << this->data.betRoundData.currentBet << " and pot " << this->data.roundData.pot;

    // TODO bet round result
    return OutEnum::ROUND_CONTINUE;
}

bool Game::betRoundContinue(short firstChecker) const noexcept {
    return !this->currentPlayerActive() ||                                                                                 // player is out of the game or folded, should be skipped
           this->data.betRoundData.currentBet != this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] ||  // current bet is not called by the player
           (this->data.betRoundData.currentBet == 0 && firstChecker != this->data.betRoundData.playerPos) ||               // current bet is 0 and the current player is not the first checker
           this->currentPlayerCanOnlyRaiseOrCall();
}

bool Game::currentPlayerActive() const noexcept { return !this->data.gameData.playerOut[this->data.betRoundData.playerPos] && !this->data.roundData.playerFolded[this->data.betRoundData.playerPos]; }

bool Game::currentPlayerCanOnlyRaiseOrCall() const noexcept {
    // current bet is the big blind and the current player is the big blind and it is the preflop round (in the preflop round the big blind can raise)
    return this->data.roundData.betRoundState == BetRoundState::PREFLOP && this->data.betRoundData.currentBet == this->data.roundData.bigBlind &&
           this->data.betRoundData.playerPos == this->data.roundData.bigBlindPos;
}

OutEnum Game::playerTurn(short& firstChecker) {
    Action action = this->players[this->data.betRoundData.playerPos]->turn(this->data);
    OutEnum res = OutEnum::ROUND_CONTINUE;
    switch (action.action) {
        case Actions::FOLD:
            PLOG_DEBUG << this->getPlayerInfo() << " folded";
            res = playerFolded();
            if (res != OutEnum::ROUND_CONTINUE) return res;
            break;

        case Actions::CHECK:
            PLOG_DEBUG << this->getPlayerInfo() << " checked";
            if (this->data.betRoundData.currentBet != 0) {
                // illegal move leads to loss of the game
                res = playerOut();
                if (res != OutEnum::ROUND_CONTINUE) return res;
            } else {
                if (firstChecker == -1) firstChecker = this->data.betRoundData.playerPos;
                this->data.nextPlayer();
            }
            break;

        case Actions::CALL:
            PLOG_DEBUG << this->getPlayerInfo() << " called";
            if (!this->bet(this->data.betRoundData.currentBet)) {
                // illegal move leads to loss of the game
                res = playerOut();
                if (res != OutEnum::ROUND_CONTINUE) return res;
            }
            break;

        case Actions::RAISE:
            PLOG_DEBUG << this->getPlayerInfo() << " raised to " << action.bet;
            if (this->data.betRoundData.currentBet == 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                res = playerOut();
                if (res != OutEnum::ROUND_CONTINUE) return res;
            }
            break;

        case Actions::BET:
            PLOG_DEBUG << this->getPlayerInfo() << " bet " << action.bet;
            if (this->data.betRoundData.currentBet != 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                res = playerOut();
                if (res != OutEnum::ROUND_CONTINUE) return res;
            }
            break;

        default:
            throw std::logic_error("Invalid action");
            PLOG_FATAL << "Invalid action: " << static_cast<int>(action.action);
    }
    return OutEnum::ROUND_CONTINUE;
}

OutEnum Game::playerTurnOnlyRaise() {
    Action action = this->players[this->data.betRoundData.playerPos]->turn(this->data, true);
    OutEnum res = OutEnum::ROUND_CONTINUE;
    switch (action.action) {
        case Actions::CALL:
            PLOG_DEBUG << this->getPlayerInfo() << " called";
            if (!this->bet(this->data.betRoundData.currentBet)) {
                // this move is not adding chips to the pot, so it can not be illegal
                PLOG_FATAL << "Player " << this->data.betRoundData.playerPos << " called but could not bet";
            }
            break;

        case Actions::RAISE:
            PLOG_DEBUG << this->getPlayerInfo() << " raised to " << action.bet;
            if (this->data.betRoundData.currentBet == 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                res = playerOut();
                if (res != OutEnum::ROUND_CONTINUE) return res;
            }
            break;

        default:
            // illegal move leads to loss of the game
            res = playerOut();
            if (res != OutEnum::ROUND_CONTINUE) return res;
    }
    return OutEnum::ROUND_CONTINUE;
}

bool Game::bet(const u_int64_t amount, const bool isBlind) noexcept {
    // amount is the whole bet, not the amount that is added to the pot
    if (!isBlind && ((amount < this->data.betRoundData.currentBet) ||                                             // call condition
        ((amount > this->data.betRoundData.currentBet) && (amount < this->data.betRoundData.currentBet + this->data.betRoundData.minimumRaise)) ||  // raise condition
        (amount < this->data.roundData.smallBlind))                                                               // bet condition
    )
        return false;
    u_int64_t addAmount = amount - this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];
    bool success = this->data.removeChips(addAmount);
    if (!success) return false;
    // minimum raise is the difference between the current bet and the new bet but at least the big blind
    if(amount > this->data.betRoundData.currentBet)
        this->data.betRoundData.minimumRaise = amount - this->data.betRoundData.currentBet;
    if(this->data.betRoundData.minimumRaise < this->data.roundData.bigBlind) this->data.betRoundData.minimumRaise = this->data.roundData.bigBlind;
    this->data.betRoundData.currentBet = amount;
    this->data.roundData.pot += addAmount;
    this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] = amount;
    this->data.nextPlayer();
    return true;
}

OutEnum Game::playerOut() noexcept {
    PLOG_WARNING << this->getPlayerInfo() << " is out";
    this->data.gameData.numNonOutPlayers--;
    this->data.roundData.numNonFoldedPlayers--;
    this->data.gameData.playerOut[this->data.betRoundData.playerPos] = true;
    this->data.nextPlayer();

    return this->getOutEnum();
}

OutEnum Game::playerFolded() noexcept {
    this->data.roundData.numNonFoldedPlayers--;
    this->data.roundData.playerFolded[this->data.betRoundData.playerPos] = true;
    this->data.nextPlayer();
    // if only one player is left, he wins the pot
    return this->getOutEnum();
}

OutEnum Game::getOutEnum() const noexcept {
    if (this->data.gameData.numNonOutPlayers == 1) {
        // only one player is left in the game, he wins the game
        return OutEnum::GAME_WON;
    } else if (this->data.roundData.numNonFoldedPlayers == 1) {
        // only one player is left in the round, he wins the pot
        return OutEnum::ROUND_WON;
    } else {
        return OutEnum::ROUND_CONTINUE;
    }
}

void Game::preflop() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::PREFLOP;
    PLOG_DEBUG << "Starting PREFLOP bet round";
    this->data.roundData.result = this->betRound();
}

void Game::flop() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::FLOP;
    this->setupBetRound();
    PLOG_DEBUG << "Starting FLOP bet round";
    for (u_int8_t i = 0; i < 3; i++) {
        this->data.roundData.communityCards[i] = this->deck.draw();  // draw flop cards
    }
    this->data.roundData.result = this->betRound();
}

void Game::turn() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::TURN;
    this->setupBetRound();
    PLOG_DEBUG << "Starting TURN bet round";
    this->data.roundData.communityCards[3] = this->deck.draw();  // draw turn card
    this->data.roundData.result = this->betRound();
}

void Game::river() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::RIVER;
    this->setupBetRound();
    PLOG_DEBUG << "Starting RIVER bet round";
    this->data.roundData.communityCards[4] = this->deck.draw();  // draw river card
    this->data.roundData.result = this->betRound();
}
