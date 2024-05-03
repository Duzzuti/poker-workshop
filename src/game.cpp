#include "game.h"

#include <algorithm>

#include "check_player/check_player.h"
#include "hand_strengths.h"
#include "human_player/human_player.h"
#include "rand_player/rand_player.h"

void Game::run(const bool initPlayers) {
    // config players
    // init players
    if (initPlayers) {
        this->players[0] = std::move(std::make_unique<CheckPlayer>(1));
        this->players[1] = std::move(std::make_unique<RandPlayer>(2));
        this->players[2] = std::move(std::make_unique<CheckPlayer>(3));
        this->players[3] = std::move(std::make_unique<RandPlayer>(4));
        this->players[4] = std::move(std::make_unique<RandPlayer>(5));
    }

    this->data.numPlayers = this->config.numPlayers;
    // reset winners
    std::memset(this->data.gameData.gameWins, 0, sizeof(this->data.gameData.gameWins));
    std::memset(this->data.gameData.chipWins, 0, sizeof(this->data.gameData.chipWins));
    std::memset(this->data.gameData.chipWinsAmount, 0, sizeof(this->data.gameData.chipWinsAmount));

    this->winnerString[0] = '\0';

    // run for the number of games specified in the config
    for (this->game = 0; this->game < this->config.numGames; this->game++) {
        // ONE GAME
        // shuffle players
        PLOG_DEBUG << "Starting game " << this->game;
        this->initPlayerOrder();
        this->data.gameData.numNonOutPlayers = this->config.numPlayers;
        // reset player out
        std::memset(this->data.gameData.playerOut, 0, sizeof(this->data.gameData.playerOut));
        for (u_int8_t i = 0; i < this->config.numPlayers; i++) this->data.gameData.playerChips[i] = this->config.startingChips[i];
        this->round = -1;

        while (this->data.gameData.numNonOutPlayers > 1) {
            if (this->config.maxRounds >= 0 && this->round >= this->config.maxRounds - 1) {
                // find the player with the most chips
                u_int8_t maxChipsPlayers[MAX_PLAYERS] = {0};
                u_int8_t maxChipsPlayersCount = 1;
                for (u_int8_t i = 1; i < this->config.numPlayers; i++) {
                    if (this->data.gameData.playerChips[i] > this->data.gameData.playerChips[maxChipsPlayers[0]]) {
                        maxChipsPlayers[0] = i;
                        maxChipsPlayersCount = 1;
                    } else if (this->data.gameData.playerChips[i] == this->data.gameData.playerChips[maxChipsPlayers[0]]) {
                        maxChipsPlayers[maxChipsPlayersCount++] = i;
                    }
                }
                // set the players with the most chips as the winner
                for (u_int8_t i = 0; i < maxChipsPlayersCount; i++) this->data.gameData.gameWins[maxChipsPlayers[i]]++;
                this->data.roundData.result = OutEnum::GAME_WON;
                this->winnerString[0] = '\0';
                for (u_int8_t i = 0; i < maxChipsPlayersCount; i++) {
                    // depending MAX_POT_DIST_STRING_LENGTH
                    std::strncat(this->winnerString, this->getPlayerInfo(maxChipsPlayers[i]), MAX_GET_PLAYER_INFO_LENGTH);
                    if (i != maxChipsPlayersCount - 1) std::strcat(this->winnerString, ", ");
                }
                PLOG_INFO << "Game " << this->game << " ended in round " << this->round << "\nWINNER IS " << this->winnerString << "\n\n";
                break;
            }
            // ONE ROUND
            this->round++;
            if (this->config.shuffleDeck)
                this->deck = Deck();
            else
                this->deck.reset();
            this->data.roundData.betRoundState = BetRoundState::PREFLOP;
            this->data.roundData.result = OutEnum::ROUND_CONTINUE;
            this->data.roundData.numActivePlayers = this->data.gameData.numNonOutPlayers;
            PLOG_DEBUG << "Starting round " << this->round;
            this->startRound();
            // PREFLOP
            this->preflop();
            // FLOP
            this->flop();
            // TURN
            this->turn();
            // RIVER
            this->river();

            if (this->data.roundData.result == OutEnum::ROUND_WON) {
                // switch to the winner
                this->data.nextActiveOrAllInPlayer();
                PLOG_DEBUG << "Pot of " << this->data.roundData.pot << " won by " << this->getPlayerInfo(MAX_PLAYERS, this->data.roundData.pot) << ". Starting new round";
                this->data.gameData.playerChips[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                this->data.gameData.chipWins[this->data.betRoundData.playerPos]++;
                this->data.gameData.chipWinsAmount[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                continue;
            } else if (this->data.roundData.result == OutEnum::GAME_WON) {
                // switch to the winner
                this->data.nextActiveOrAllInPlayer();
                this->data.gameData.playerChips[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                this->data.gameData.gameWins[this->data.betRoundData.playerPos]++;
                this->data.gameData.chipWins[this->data.betRoundData.playerPos]++;
                this->data.gameData.chipWinsAmount[this->data.betRoundData.playerPos] += this->data.roundData.pot;
                PLOG_INFO << "Game " << this->game << " ended in round " << this->round << "\nWINNER IS " << this->getPlayerInfo() << "\n\n";
                break;
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
                if (this->data.roundData.playerFolded[i] || this->data.gameData.playerOut[i]) continue;
                PLOG_DEBUG << this->getPlayerInfo(i) << " has hand " << this->players[i]->getHand().first.toString() << " " << this->players[i]->getHand().second.toString() << " and hand strength "
                           << EnumToString::enumToString(handStrengths[i].handkind) << " " << handStrengths[i].rankStrength;
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
            winnerString[0] = '\0';
            for (u_int8_t i = 0; i < numWinners; i++) {
                // depending MAX_POT_DIST_STRING_LENGTH
                std::strncat(winnerString, this->getPlayerInfo(winners[i], potPerWinner), MAX_GET_PLAYER_INFO_LENGTH);
                this->data.gameData.playerChips[winners[i]] += potPerWinner;
                this->data.gameData.chipWins[winners[i]]++;
                this->data.gameData.chipWinsAmount[winners[i]] += potPerWinner;
                if (i != numWinners - 1) std::strcat(winnerString, ", ");
            }
            PLOG_DEBUG << "Pot of " << this->data.roundData.pot << " won by " << winnerString << ". Starting new round";
            // check if one player has 0 chips and is out
            if (this->data.roundData.numAllInPlayers != 0) {
                for (u_int8_t i = 0; i < this->data.numPlayers; i++) {
                    if (this->data.gameData.playerChips[i] == 0 && !this->data.gameData.playerOut[i]) {
                        this->data.gameData.numNonOutPlayers--;
                        this->data.gameData.playerOut[i] = true;
                        PLOG_WARNING << this->getPlayerInfo(i) << " is out of chips and is out";
                    }
                }
                if (this->data.gameData.numNonOutPlayers == 1) {
                    // only one player is left in the game, he wins the game
                    this->data.gameData.gameWins[winners[0]]++;
                    this->data.roundData.result = OutEnum::GAME_WON;
                    PLOG_INFO << "Game " << game << " ended in round " << round << "\nWINNER IS " << this->getPlayerInfo(winners[0]) << "\n\n";
                    break;
                }
            }
        }
    }
    PLOG_INFO << "Statistics: \n";
    // sort players by wins
    std::pair<u_int8_t, u_int32_t> winners[this->data.numPlayers];
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) winners[i] = std::make_pair(i, this->data.gameData.gameWins[i]);
    std::sort(&winners[0], &winners[this->config.numPlayers], [](const std::pair<u_int8_t, u_int32_t>& a, const std::pair<u_int8_t, u_int32_t>& b) { return a.second > b.second; });
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) PLOG_INFO << STR_PLAYER << this->players[winners[i].first]->getName() << " won " << winners[i].second << " games";
    PLOG_INFO << "\n";
}

const char* Game::getPlayerInfo(u_int8_t playerPos, const int64_t chipsDiff, const int64_t baseChipsDiff) const noexcept {
    // create player info string with MAX_GET_PLAYER_INFO_LENGTH
    static char playerInfo[MAX_GET_PLAYER_INFO_LENGTH];
    // if playerPos == MAX_PLAYERS, use the current player
    if (playerPos == MAX_PLAYERS) playerPos = this->data.betRoundData.playerPos;
    // if chipsDiff == 0, do not add chipsDiff
    if (chipsDiff == 0)
        std::snprintf(playerInfo, sizeof(playerInfo), "%s%s[%lu]", STR_PLAYER, this->players[playerPos]->getName(), this->data.gameData.playerChips[playerPos] + baseChipsDiff);
    else
        std::snprintf(playerInfo, sizeof(playerInfo), "%s%s[%lu%s%lu]", STR_PLAYER, this->players[playerPos]->getName(), this->data.gameData.playerChips[playerPos] + baseChipsDiff,
                      chipsDiff > 0 ? " + " : " - ", std::abs(chipsDiff));
    return playerInfo;
}

void Game::initPlayerOrder() noexcept {
    // shuffle player order
    if (this->config.shufflePlayers) std::random_shuffle(&this->players[0], &this->players[this->config.numPlayers]);
    PLOG_INFO << "Shuffled players, new order:";
    // rearrange the game stats according to the new player order
    u_int32_t gameWinsCopy[MAX_PLAYERS];
    u_int32_t chipWinsCopy[MAX_PLAYERS];
    u_int64_t chipWinsAmountCopy[MAX_PLAYERS];
    std::memcpy(gameWinsCopy, this->data.gameData.gameWins, sizeof(gameWinsCopy));
    std::memcpy(chipWinsCopy, this->data.gameData.chipWins, sizeof(chipWinsCopy));
    std::memcpy(chipWinsAmountCopy, this->data.gameData.chipWinsAmount, sizeof(chipWinsAmountCopy));
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) {
        // set playerPosNum for each player
        this->data.gameData.gameWins[i] = gameWinsCopy[this->players[i]->getPlayerPosNum()];
        this->data.gameData.chipWins[i] = chipWinsCopy[this->players[i]->getPlayerPosNum()];
        this->data.gameData.chipWinsAmount[i] = chipWinsAmountCopy[this->players[i]->getPlayerPosNum()];
        this->players[i]->setPlayerPosNum(i);
        PLOG_INFO << this->players[i]->getName();
    }
}

void Game::setBlinds() noexcept {
    // blinds
    if (this->data.gameData.numNonOutPlayers == 2) {
        // heads up rule (small blind is the dealer)
        // adjust button if the big blind would be the same player again
        this->data.roundData.dealerPos = this->data.roundData.bigBlindPos;
        this->data.betRoundData.playerPos = this->data.roundData.dealerPos;
    }
    // if the player can not bet the small blind, he is all-in
    this->data.roundData.smallBlindPos = this->data.betRoundData.playerPos;
    const u_int64_t smallBlindBet = this->betBlind(this->data.roundData.smallBlind);
    if (this->data.getChips() == 0)
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -smallBlindBet, smallBlindBet) << " is all-in with small blind " << this->data.roundData.smallBlind;
    else
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -smallBlindBet, smallBlindBet) << " bets small blind " << this->data.roundData.smallBlind;

    this->data.nextActivePlayer();

    // big blind analog to small blind
    this->data.roundData.bigBlindPos = this->data.betRoundData.playerPos;
    this->data.roundData.bigBlindBet = this->betBlind(this->data.roundData.bigBlind);
    if (this->data.getChips() == 0)
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -this->data.roundData.bigBlindBet, this->data.roundData.bigBlindBet) << " is all-in with big blind " << this->data.roundData.bigBlind;
    else
        PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -this->data.roundData.bigBlindBet, this->data.roundData.bigBlindBet) << " bets big blind " << this->data.roundData.bigBlind;

    if (this->data.roundData.numActivePlayers == this->data.roundData.numAllInPlayers) {
        // all players are all-in, skip to showdown
        this->data.roundData.result = OutEnum::ROUND_SHOWDOWN;
        return;
    }
    this->data.nextActivePlayer();
    this->data.roundData.result = this->checkRoundSkip();
    this->equalizeMove();
}

void Game::setupBetRound() noexcept {
    // resets the bet round data, first to act is the player after the dealer
    this->data.betRoundData.playerPos = this->data.roundData.dealerPos;
    this->data.nextActivePlayer();
    // reset player bets
    std::memset(this->data.betRoundData.playerBets, 0, sizeof(this->data.betRoundData.playerBets));
    this->data.betRoundData.currentBet = 0;
}

void Game::startRound() {
    const bool firstRound = this->round == 0;
    // reset deck and round data
    if (this->config.shuffleDeck) this->deck.shuffle();
    // select new dealer
    u_int8_t lastDealerPos = this->data.roundData.dealerPos;
    this->data.selectDealer(firstRound);

    this->data.roundData.addBlind = this->config.addBlindPerDealer0;
    if (firstRound) this->data.roundData.smallBlind = this->config.smallBlind;
    // if the dealer is at position 0 again (or skipped 0), add the addBlind amount
    else if (this->data.roundData.dealerPos < lastDealerPos)
        this->data.roundData.smallBlind += this->config.addBlindPerDealer0;
    // big blind is always double the small blind
    this->data.roundData.bigBlind = this->data.roundData.smallBlind * 2;
    this->data.roundData.pot = 0;
    this->data.roundData.numAllInPlayers = 0;
    // reset player folded
    std::memset(this->data.roundData.playerFolded, 0, sizeof(this->data.roundData.playerFolded));
    std::memset(this->data.roundData.playerBetsTotal, 0, sizeof(this->data.roundData.playerBetsTotal));
    this->setupBetRound();

    // deal cards
    for (u_int8_t i = 0; i < this->config.numPlayers; i++) {
        if (this->data.gameData.playerOut[i]) continue;
        this->players[i]->setHand(this->deck.draw(), this->deck.draw());
    }
    // first action is setting the blinds
    if (firstRound) this->data.roundData.bigBlindPos = 0;  // fix first round heads up
    this->setBlinds();
}

OutEnum Game::betRound() {
    // position of the first player that checked or MAX_PLAYERS if no player checked yet
    u_int8_t firstChecker = MAX_PLAYERS;
    // TODO last Raiser
    // TODO edge case where the big blind is all in with less chips than the big blind, therefore currentBet is not actually the current bet
    this->lastRaiser = MAX_PLAYERS;
    this->data.betRoundData.minimumRaise = this->data.betRoundData.currentBet < this->data.roundData.bigBlind ? this->data.roundData.bigBlind : this->data.betRoundData.currentBet * 2;
    // this loop will run until all players have either folded, checked or called
    // we can only exit if it is a players turn and he is in the game, has the same bet as the current bet and all players have checked if the bet is 0
    // we need to consider the case where every player folds except one, then the last player wins the pot
    while (true) {
        if (this->currentPlayerBlindOption()) {  // current player is the live big blind in the preflop round
            // player has the blind option
            const OutEnum turnRes = this->playerTurnBlindOption();
            if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
        } else if (this->data.betRoundData.currentBet != this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] ||  // current bet is not called by the player
                   (this->data.betRoundData.currentBet == 0 && firstChecker != this->data.betRoundData.playerPos)) {               // current bet is 0 and the current player is not the first checker
            if (this->lastRaiser == this->data.betRoundData.playerPos) {
                // last raiser has to equalize the all-in bet
                const OutEnum turnRes = this->playerTurnEqualize();
                if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
            } else {
                // first player is used as the first raiser (to cover the case where no player raises)
                if (this->lastRaiser == MAX_PLAYERS) this->lastRaiser = this->data.betRoundData.playerPos;
                // player has to make a turn (fold, check, call, raise, bet)
                const OutEnum turnRes = this->playerTurn(firstChecker);
                if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
            }
        } else
            break;
    }

    PLOG_DEBUG << "Bet round finished with bet " << this->data.betRoundData.currentBet << " and pot " << this->data.roundData.pot;

    // TODO bet round result
    return OutEnum::ROUND_CONTINUE;
}

OutEnum Game::checkRoundSkip() const noexcept {
    // checks if one player is not all-in. That will cause a skip to the showdown
    if (this->data.roundData.numActivePlayers == this->data.roundData.numAllInPlayers + 1) {
        // all players, except one, are all-in, skip to showdown
        return OutEnum::ROUND_SHOWDOWN;
    }
    return OutEnum::ROUND_CONTINUE;
    // TODO: add playerTurnEqualize with options fold, call (bet equalizer), all-in (max equalizer bet) for the last active player who has to equalize the all-in bet
}

bool Game::currentPlayerBlindOption() const noexcept {
    // current bet is the big blind and the current player is the big blind and it is the preflop round (the big blind can raise in the preflop round)
    return this->data.roundData.betRoundState == BetRoundState::PREFLOP && this->data.betRoundData.currentBet == this->data.roundData.bigBlind &&
           this->data.betRoundData.playerPos == this->data.roundData.bigBlindPos;
}

OutEnum Game::playerTurn(u_int8_t& firstChecker) {
    // get action from player
    Action action = this->players[this->data.betRoundData.playerPos]->turn(this->data);
    u_int64_t allInAmount;
    u_int64_t callAdd;
    u_int64_t raiseAdd;
    // store the error message if the action is illegal
    char str[MAX_ACTION_ERROR_LENGTH];
    switch (action.action) {
        case Actions::FOLD:
            // player folded
            PLOG_DEBUG << this->getPlayerInfo() << " folded";
            return playerFolded();

        case Actions::CHECK:
            // player checked
            if (this->data.betRoundData.currentBet != 0) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_CHECK_ERROR, this->data.betRoundData.currentBet);
                return playerOut(str);
            }
            PLOG_DEBUG << this->getPlayerInfo() << " checked";
            // if the player is the first checker, set firstChecker to the player position
            if (firstChecker == MAX_PLAYERS) firstChecker = this->data.betRoundData.playerPos;
            this->data.nextActivePlayer();
            break;

        case Actions::CALL:
            // player called
            callAdd = this->data.getCallAdd();
            if (!this->bet(this->data.betRoundData.currentBet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_CALL_ERROR, this->data.betRoundData.currentBet);
                return playerOut(str);
            }
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -callAdd, callAdd) << " called";
            this->data.nextActivePlayer();
            break;

        case Actions::RAISE:
            // player raised
            raiseAdd = this->data.getRaiseAdd(action.bet);
            if (this->data.betRoundData.currentBet == 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_RAISE_ERROR, action.bet);
                return playerOut(str);
            }
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -raiseAdd, raiseAdd) << " raised to " << action.bet;
            this->data.nextActivePlayer();
            break;

        case Actions::BET:
            // player bet
            if (this->data.betRoundData.currentBet != 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_BET_ERROR, action.bet);
                return playerOut(str);
            }
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -action.bet, action.bet) << " bet " << action.bet;
            this->data.nextActivePlayer();
            break;

        case Actions::ALL_IN:
            // player is all-in
            this->data.roundData.numAllInPlayers++;
            allInAmount = this->data.getChips();
            this->data.addPlayerBet(allInAmount);
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -allInAmount) << " is all-in with " << this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];
            this->data.removeChipsAllIn();
            if (this->data.betRoundData.currentBet < this->data.betRoundData.playerBets[this->data.betRoundData.playerPos]) {
                // TODO raise all-in
                // set the current bet to the all-in amount, while also leaving the minimum raise unchanged if the all-in amount is not a raise
                this->adaptRaiseAttributes(allInAmount);
                this->data.betRoundData.currentBet = this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];
            }
            this->data.nextActivePlayer();
            return this->checkRoundSkip();

        default:
            // some action is not handled by the switch statement
            throw std::logic_error("Invalid action");
            PLOG_FATAL << "Invalid action: " << static_cast<int>(action.action);
    }
    return OutEnum::ROUND_CONTINUE;
}

OutEnum Game::playerTurnBlindOption() {
    // get action from player
    Action action = this->players[this->data.betRoundData.playerPos]->turn(this->data, true);
    u_int64_t raiseAdd;
    u_int64_t allInAmount;
    // store the error message if the action is illegal
    char str[MAX_ACTION_ERROR_LENGTH_ONLY_RAISE];
    switch (action.action) {
        case Actions::CALL:
            // player called, does not add chips to the pot
            PLOG_DEBUG << this->getPlayerInfo() << " called";
            if (!this->bet(this->data.betRoundData.currentBet)) {
                // this move is not adding chips to the pot, so it can not be illegal
                PLOG_FATAL << "Player " << this->data.betRoundData.playerPos << " called but could not bet";
                throw std::logic_error("Player called but could not bet");
                return OutEnum::ROUND_CONTINUE;
            }
            this->data.nextActivePlayer();
            return OutEnum::ROUND_CONTINUE;

        case Actions::RAISE:
            // player raised
            raiseAdd = this->data.getRaiseAdd(action.bet);
            if (this->data.betRoundData.currentBet == 0 || !this->bet(action.bet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_RAISE_ERROR, action.bet);
                return playerOut(str);
            }
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -raiseAdd, raiseAdd) << " raised to " << action.bet;
            this->data.nextActivePlayer();
            return OutEnum::ROUND_CONTINUE;

        case Actions::ALL_IN:
            // player is all-in
            this->data.roundData.numAllInPlayers++;
            allInAmount = this->data.getChips();
            this->data.addPlayerBet(allInAmount);
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -allInAmount) << " is all-in with " << this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];
            this->data.removeChipsAllIn();
            // TODO raise all-in
            // set the current bet to the all-in amount, while also leaving the minimum raise unchanged if the all-in amount is not a raise
            this->adaptRaiseAttributes(this->data.betRoundData.playerBets[this->data.betRoundData.playerPos]);
            this->data.betRoundData.currentBet = this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];

            this->data.nextActivePlayer();
            return this->checkRoundSkip();

        default:
            // illegal move leads to loss of the game
            // set up error message
            std::snprintf(str, sizeof(str), "%s%i", STR_ACTION_ERROR, static_cast<int>(action.action));
            return playerOut(str);
    }
}

OutEnum Game::playerTurnEqualize() noexcept {
    // get equalize action from player
    Action action = this->players[this->data.betRoundData.playerPos]->turn(this->data, false, true);
    u_int64_t callAdd;
    u_int64_t allInAmount;
    // store the error message if the action is illegal
    char str[MAX_ACTION_ERROR_LENGTH_ONLY_RAISE];
    switch (action.action) {
        case Actions::CALL:
            // player called or bet to equalize the all-in bet
            callAdd = this->data.getCallAdd();
            if (!this->bet(this->data.betRoundData.currentBet)) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_CALL_ERROR, this->data.betRoundData.currentBet);
                return playerOut(str);
            }
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -callAdd, callAdd) << " called";
            this->data.tryNextActivePlayer();
            return OutEnum::ROUND_CONTINUE;

        case Actions::FOLD:
            // player folded
            PLOG_DEBUG << this->getPlayerInfo() << " folded";
            return playerFolded();

        case Actions::ALL_IN:
            // player is all-in, needs to be at maximum the all-in amount
            allInAmount = this->data.getChips();
            if (this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] + allInAmount > this->data.betRoundData.currentBet) {
                // illegal move leads to loss of the game
                // set up error message
                std::snprintf(str, sizeof(str), "%s%lu", STR_ALL_IN_ERROR, this->data.betRoundData.playerBets[this->data.betRoundData.playerPos] + allInAmount);
                return playerOut(str);
            }
            this->data.addPlayerBet(allInAmount);
            PLOG_DEBUG << this->getPlayerInfo(MAX_PLAYERS, -allInAmount) << " is all-in with " << this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];
            this->data.removeChipsAllIn();
            this->data.roundData.numAllInPlayers++;
            this->data.tryNextActivePlayer();
            return this->checkRoundSkip();

        default:
            // illegal move leads to loss of the game
            // set up error message
            std::snprintf(str, sizeof(str), "%s%i", STR_ACTION_ERROR, static_cast<int>(action.action));
            return playerOut(str);
    }
}

u_int64_t Game::betBlind(const u_int64_t blind) noexcept {
    // bet the blind, if the player can not bet the blind, he is all-in
    const bool success = this->data.removeChips(blind);
    this->data.betRoundData.currentBet = blind;
    if (!success) {
        // TODO player is all-in
        this->data.roundData.numAllInPlayers++;
        const u_int64_t allInAmount = this->data.getChips();
        this->data.removeChipsAllIn();
        this->data.addPlayerBet(allInAmount);
        return allInAmount;
    }
    this->data.addPlayerBet(blind);
    return blind;
}

bool Game::bet(const u_int64_t amount) noexcept {
    // amount is the whole bet, not the amount that is added to the pot
    if (((amount == this->data.betRoundData.currentBet && this->data.betRoundData.currentBet == 0) || (amount < this->data.betRoundData.currentBet) ||  // call condition
         ((amount > this->data.betRoundData.currentBet) && (amount < this->data.betRoundData.minimumRaise)) ||                                          // raise condition
         (amount < this->data.roundData.bigBlind && this->data.betRoundData.currentBet >= this->data.roundData.bigBlind))                               // bet condition
    )
        return false;
    const u_int64_t addAmount = amount - this->data.betRoundData.playerBets[this->data.betRoundData.playerPos];
    const bool success = this->data.removeChips(addAmount);
    if (!success) return false;
    this->adaptRaiseAttributes(amount);
    this->data.betRoundData.currentBet = amount;
    this->data.addPlayerBet(addAmount);
    return true;
}

OutEnum Game::playerOut(const char* reason) noexcept {
    // player is out of the game, log the reason and set the player out
    PLOG_WARNING << this->getPlayerInfo() << " " << reason << " and is out";
    this->data.gameData.numNonOutPlayers--;
    this->data.roundData.numActivePlayers--;
    this->data.gameData.playerOut[this->data.betRoundData.playerPos] = true;
    this->data.gameData.playerChips[this->data.betRoundData.playerPos] = 0;
    this->data.tryNextActivePlayer();

    return this->getOutEnum();
}

OutEnum Game::playerFolded() noexcept {
    this->data.roundData.numActivePlayers--;
    this->data.roundData.playerFolded[this->data.betRoundData.playerPos] = true;
    this->data.tryNextActivePlayer();
    // if only one player is left, he wins the pot
    return this->getOutEnum();
}

OutEnum Game::getOutEnum() const noexcept {
    if (this->data.gameData.numNonOutPlayers == 1) {
        // only one player is left in the game, he wins the game
        return OutEnum::GAME_WON;
    } else if (this->data.roundData.numActivePlayers == 1) {
        // only one player is left in the round, he wins the pot
        return OutEnum::ROUND_WON;
    } else {
        return this->checkRoundSkip();
    }
}

void Game::equalizeMove() noexcept {
    if (this->data.roundData.result == OutEnum::ROUND_SHOWDOWN) {
        // check if the current bet is equalized by the last player
        this->data.nextActivePlayer();
        if (this->data.betRoundData.currentBet != this->data.betRoundData.playerBets[this->data.betRoundData.playerPos]) {
            if (this->data.roundData.betRoundState == BetRoundState::PREFLOP && this->data.roundData.bigBlindBet != this->data.roundData.bigBlind &&
                this->data.roundData.bigBlind == this->data.betRoundData.currentBet) {
                // attention for the edge cases where the big blind is all-in with less chips than the big blind, therefore currentBet is not actually the current bet
                // look for the highest bet in the preflop round
                u_int64_t highestBet = 0;
                u_int8_t highestBetPlayer = MAX_PLAYERS;
                for (u_int8_t i = 0; i < this->data.numPlayers; i++) {
                    if (this->data.betRoundData.playerBets[i] > highestBet) {
                        highestBet = this->data.betRoundData.playerBets[i];
                        highestBetPlayer = i;
                    }
                }
                this->data.betRoundData.currentBet = highestBet;
                if (highestBetPlayer == this->data.betRoundData.playerPos) {
                    // the player is already the highest better
                    this->data.roundData.result = OutEnum::ROUND_SHOWDOWN;
                    return;
                }
            }
            // the last player has to equalize the all-in bet
            const OutEnum res = this->playerTurnEqualize();
            // if only one player is all-in and the last one folds or is out the game or round could be won
            if (res != OutEnum::ROUND_CONTINUE) this->data.roundData.result = res;
        }
    }
}

void Game::preflop() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) return;
    this->data.roundData.betRoundState = BetRoundState::PREFLOP;
    PLOG_DEBUG << "Starting PREFLOP bet round";
    this->data.roundData.result = this->betRound();
    this->equalizeMove();
}

void Game::flop() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) {
        if (this->data.roundData.result == OutEnum::ROUND_SHOWDOWN) {
            for (u_int8_t i = 0; i < 3; i++) {
                this->data.roundData.communityCards[i] = this->deck.draw();  // draw flop cards
            }
        }
        return;
    }
    for (u_int8_t i = 0; i < 3; i++) {
        this->data.roundData.communityCards[i] = this->deck.draw();  // draw flop cards
    }
    this->data.roundData.betRoundState = BetRoundState::FLOP;
    this->setupBetRound();
    PLOG_DEBUG << "Starting FLOP bet round";
    this->data.roundData.result = this->betRound();
    this->equalizeMove();
}

void Game::turn() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) {
        if (this->data.roundData.result == OutEnum::ROUND_SHOWDOWN) {
            this->data.roundData.communityCards[3] = this->deck.draw();  // draw turn card
        }
        return;
    }
    this->data.roundData.communityCards[3] = this->deck.draw();  // draw turn card
    this->data.roundData.betRoundState = BetRoundState::TURN;
    this->setupBetRound();
    PLOG_DEBUG << "Starting TURN bet round";
    this->data.roundData.result = this->betRound();
    this->equalizeMove();
}

void Game::river() {
    if (this->data.roundData.result != OutEnum::ROUND_CONTINUE) {
        if (this->data.roundData.result == OutEnum::ROUND_SHOWDOWN) {
            this->data.roundData.communityCards[4] = this->deck.draw();  // draw river card
            this->data.roundData.betRoundState = BetRoundState::RIVER;
        }
        return;
    }
    this->data.roundData.communityCards[4] = this->deck.draw();  // draw river card
    this->data.roundData.betRoundState = BetRoundState::RIVER;
    this->setupBetRound();
    PLOG_DEBUG << "Starting RIVER bet round";
    this->data.roundData.result = this->betRound();
    this->equalizeMove();
}

bool Game::adaptRaiseAttributes(const u_int64_t amount) noexcept {
    // returns whether the raise was a full bet
    // minimum raise is the difference between the current bet and the new bet but at least the big blind plus the new bet
    const u_int64_t maxNextRaise = std::max(2 * amount - this->data.betRoundData.currentBet, amount + this->data.roundData.bigBlind);
    if (amount >= this->data.betRoundData.minimumRaise) {
        this->data.betRoundData.minimumRaise = maxNextRaise;
        this->lastRaiser = this->data.betRoundData.playerPos;
        return true;
    }
    // adjust the minimum raise if necessary
    this->data.betRoundData.minimumRaise = std::max(this->data.betRoundData.minimumRaise, maxNextRaise);
    return false;
}
