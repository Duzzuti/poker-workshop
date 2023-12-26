#include "game.h"

#include <algorithm>

#include "check_player/check_player.h"
#include "deck.h"
#include "hand_strengths.h"
#include "logger.h"
#include "rand_player/rand_player.h"
#include "utils.h"

void Game::run() const {
    // config players
    Player* players[m_config.numPlayers];
    // init players
    players[0] = new CheckPlayer(1);
    players[1] = new RandPlayer(2);
    players[2] = new CheckPlayer(3);
    players[3] = new RandPlayer(4);

    Data data;
    data.numPlayers = m_config.numPlayers;

    // run for the number of games specified in the config
    for (u_int64_t game = 0; game < m_config.numRounds; game++) {
        // ONE GAME
        // shuffle players
        PLOG_DEBUG << "Starting game " << game;
        initPlayerOrder(players, m_config.numPlayers);
        data.gameData.playerOut = std::vector<bool>(m_config.numPlayers, false);
        data.gameData.playerChips = std::vector<u_int64_t>(m_config.numPlayers, m_config.startingChips);
        int32_t round = -1;

        while (std::count(data.gameData.playerOut.begin(), data.gameData.playerOut.end(), false) > 1) {
            // ONE ROUND
            round++;
            Deck deck;
            OutEnum result = OutEnum::ROUND_CONTINUE;

            PLOG_DEBUG << "Starting round " << round;
            result = this->startRound(players, data, deck, round == 0);
            // PREFLOP
            result = this->preflop(players, data, result);
            // FLOP
            result = this->flop(players, data, deck, result);
            // TURN
            result = this->turn(players, data, deck, result);
            // RIVER
            result = this->river(players, data, deck, result);

            if (result == OutEnum::GAME_WON) {
                PLOG_INFO << "Game " << game << " ended in round " << round;
                break;
            } else if (result == OutEnum::ROUND_WON) {
                PLOG_DEBUG << "Pot won, starting new round";
                continue;
            }

            // SHOWDOWN
            PLOG_DEBUG << "SHOWDOWN!!! Community cards: " << data.roundData.communityCards[0].toString() << " " << data.roundData.communityCards[1].toString() << " "
                       << data.roundData.communityCards[2].toString() << " " << data.roundData.communityCards[3].toString() << " " << data.roundData.communityCards[4].toString();
            // get hand strength for each player
            std::vector<HandStrengths> handStrengths = HandStrengths::getHandStrengths(players, data);
            // get winner

            HandStrengths strongestHand = HandStrengths(HandKinds::NO_HAND, 0);
            std::vector<u_int8_t> winners;

            for (u_int8_t i = 0; i < data.numPlayers; i++) {
                if (data.gameData.playerOut[i] || data.roundData.playerFolded[i]) continue;
                PLOG_DEBUG << "Player " << +i << " has hand " << players[i]->getHand().first.toString() << " " << players[i]->getHand().second.toString() << " and hand strength "
                           << handStrengths[i].handkind << " " << handStrengths[i].rankStrength;
                if (handStrengths[i] > strongestHand) {
                    strongestHand = handStrengths[i];
                    winners.clear();
                    winners.push_back(i);
                } else if (handStrengths[i] == strongestHand) {
                    winners.push_back(i);
                }
            }

            // distribute pot
            u_int64_t potPerWinner = data.roundData.pot / winners.size();
            for (u_int8_t i = 0; i < (u_int8_t)winners.size(); i++) {
                data.gameData.playerChips[winners[i]] += potPerWinner;
            }
        }
    }

    // delete players, free memory
    for (u_int8_t i = 0; i < m_config.numPlayers; i++) {
        delete players[i];
    }
}

OutEnum Game::setBlinds(Player* players[], Data& data) const noexcept {
    // blinds
    OutEnum res = OutEnum::ROUND_CONTINUE;
    PLOG_DEBUG << getPlayerInfo(players, data) << " bets small blind " << data.roundData.smallBlind;
    while (!this->bet(data, data.roundData.smallBlind)) {
        res = this->playerOut(players, data);
        if (res != OutEnum::ROUND_CONTINUE) return res;
        PLOG_DEBUG << getPlayerInfo(players, data) << " bets small blind " << data.roundData.smallBlind;
    }
    PLOG_DEBUG << getPlayerInfo(players, data) << " bets big blind " << data.roundData.bigBlind;
    while (!this->bet(data, data.roundData.bigBlind)) {
        res = this->playerOut(players, data);
        if (res != OutEnum::ROUND_CONTINUE) return res;
        PLOG_DEBUG << getPlayerInfo(players, data) << " bets big blind " << data.roundData.bigBlind;
    }
    return OutEnum::ROUND_CONTINUE;
}

void Game::setupBetRound(Data& data) const noexcept {
    data.betRoundData.playerPos = data.roundData.dealerPos;
    data.nextPlayer();
    data.betRoundData.playerBets = std::vector<u_int64_t>(m_config.numPlayers, 0);
    data.betRoundData.currentBet = 0;
}

OutEnum Game::startRound(Player* players[], Data& data, Deck& deck, const bool firstRound) const noexcept {
    deck.shuffle();
    u_int8_t dealerPos;
    if (!firstRound) dealerPos = data.roundData.dealerPos;
    data.selectDealer(firstRound);

    data.roundData.addBlind = m_config.addBlindPerDealer0;
    if (firstRound)
        data.roundData.smallBlind = m_config.smallBlind;
    else if (data.roundData.dealerPos < dealerPos)  // if the dealer is at position 0 again (or skipped 0), add the addBlind amount
        data.roundData.smallBlind += m_config.addBlindPerDealer0;
    data.roundData.bigBlind = data.roundData.smallBlind * 2;
    data.roundData.pot = 0;
    data.roundData.playerFolded = std::vector<bool>(m_config.numPlayers, false);
    data.roundData.communityCards = std::vector<Card>();
    this->setupBetRound(data);

    // deal cards
    for (u_int8_t i = 0; i < m_config.numPlayers; i++) {
        if (data.gameData.playerOut[i]) continue;
        players[i]->setHand(deck.draw(), deck.draw());
    }

    return this->setBlinds(players, data);
}

OutEnum Game::betRound(Player* players[], Data& data) const {
    short firstChecker = -1;  // position of the first player that checked
    // this loop will run until all players have either folded, checked or called
    // we can only exit if it is a players turn and he is in the game, has the same bet as the current bet and all players have checked if the bet is 0
    // we need to consider the case where every player folds except one, then the last player wins the pot
    while (data.roundData.playerFolded[data.betRoundData.playerPos] || data.gameData.playerOut[data.betRoundData.playerPos] ||
           data.betRoundData.currentBet != data.betRoundData.playerBets[data.betRoundData.playerPos] || (data.betRoundData.currentBet == 0 && firstChecker != data.betRoundData.playerPos)) {
        if (data.roundData.playerFolded[data.betRoundData.playerPos] || data.gameData.playerOut[data.betRoundData.playerPos]) {
            // player is out of the game or folded, skip turn
            data.nextPlayer();
            continue;
        }
        OutEnum turnRes = this->playerTurn(players, data, &firstChecker);
        if (turnRes != OutEnum::ROUND_CONTINUE) return turnRes;
    }

    PLOG_DEBUG << "Bet round finished with bet " << data.betRoundData.currentBet << " and pot " << data.roundData.pot;

    // TODO bet round result
    return OutEnum::ROUND_CONTINUE;
}

OutEnum Game::playerTurn(Player* players[], Data& data, short* firstChecker) const {
    Action action = players[data.betRoundData.playerPos]->turn(data);
    OutEnum res = OutEnum::ROUND_CONTINUE;
    switch (action.action) {
        case Actions::FOLD:
            PLOG_DEBUG << getPlayerInfo(players, data) << " folded";
            res = playerFolded(data);
            if (res != OutEnum::ROUND_CONTINUE) return res;
            break;

        case Actions::CHECK:
            PLOG_DEBUG << getPlayerInfo(players, data) << " checked";
            if (data.betRoundData.currentBet != 0) {
                // illegal move leads to loss of the game
                res = playerOut(players, data);
                if (res != OutEnum::ROUND_CONTINUE) return res;
            } else {
                if (*firstChecker == -1) *firstChecker = data.betRoundData.playerPos;
                data.nextPlayer();
            }
            break;

        case Actions::CALL:
            PLOG_DEBUG << getPlayerInfo(players, data) << " called";
            if (!this->bet(data, data.betRoundData.currentBet)) {
                // illegal move leads to loss of the game
                res = playerOut(players, data);
                if (res != OutEnum::ROUND_CONTINUE) return res;
            }
            break;

        case Actions::RAISE:
            PLOG_DEBUG << getPlayerInfo(players, data) << " raised to " << action.bet;
            if (!this->bet(data, action.bet)) {
                // illegal move leads to loss of the game
                res = playerOut(players, data);
                if (res != OutEnum::ROUND_CONTINUE) return res;
            }
            break;

        case Actions::BET:
            PLOG_DEBUG << getPlayerInfo(players, data) << " bet " << action.bet;
            if (data.betRoundData.currentBet != 0 || !this->bet(data, action.bet)) {
                // illegal move leads to loss of the game
                res = playerOut(players, data);
                if (res != OutEnum::ROUND_CONTINUE) return res;
            }
            break;

        default:
            throw std::logic_error("Invalid action");
            PLOG_FATAL << "Invalid action: " << static_cast<int>(action.action);
    }
    return OutEnum::ROUND_CONTINUE;
}

bool Game::bet(Data& data, const u_int64_t amount) const noexcept {
    // amount is the whole bet, not the amount that is added to the pot
    if ((amount < data.betRoundData.currentBet) ||                                                   // call condition
        ((amount > data.betRoundData.currentBet) && (amount < data.betRoundData.currentBet * 2)) ||  // raise condition
        (amount < data.roundData.smallBlind)                                                         // bet condition
    )
        return false;
    u_int64_t addAmount = amount - data.betRoundData.playerBets[data.betRoundData.playerPos];
    bool success = data.removeChips(addAmount);
    if (!success) return success;
    data.betRoundData.currentBet = amount;
    data.roundData.pot += addAmount;
    data.betRoundData.playerBets[data.betRoundData.playerPos] = amount;
    data.nextPlayer();
    return true;
}

OutEnum Game::playerOut(Player* players[], Data& data) const noexcept {
    PLOG_WARNING << getPlayerInfo(players, data) << " is out";
    data.gameData.playerOut[data.betRoundData.playerPos] = true;
    data.nextPlayer();

    return this->getOutEnum(data);
}

OutEnum Game::playerFolded(Data& data) const noexcept {
    data.roundData.playerFolded[data.betRoundData.playerPos] = true;
    data.nextPlayer();
    // if only one player is left, he wins the pot
    return this->getOutEnum(data);
}

OutEnum Game::getOutEnum(const Data& data) const noexcept {
    u_int8_t numActivePlayers = 0;  // number of players that are not out and not folded
    u_int8_t numNonOutPlayers = 0;  // number of players that are not out
    for (u_int8_t i = 0; i < data.numPlayers; i++) {
        if (!data.gameData.playerOut[i]) {
            numNonOutPlayers++;
            if (!data.roundData.playerFolded[i]) numActivePlayers++;
        }
    }
    if (numNonOutPlayers == 1) {
        // only one player is left in the game, he wins the game
        return OutEnum::GAME_WON;
    } else if (numActivePlayers == 1) {
        // only one player is left in the round, he wins the pot
        return OutEnum::ROUND_WON;
    } else {
        return OutEnum::ROUND_CONTINUE;
    }
}

OutEnum Game::preflop(Player* players[], Data& data, OutEnum lastRes) const noexcept {
    if(lastRes != OutEnum::ROUND_CONTINUE) return lastRes;
    PLOG_DEBUG << "Starting PREFLOP bet round";
    return this->betRound(players, data);
}

OutEnum Game::flop(Player* players[], Data& data, Deck& deck, OutEnum lastRes) const noexcept {
    if(lastRes != OutEnum::ROUND_CONTINUE) return lastRes;
    this->setupBetRound(data);
    PLOG_DEBUG << "Starting FLOP bet round";
    for (u_int8_t i = 0; i < 3; i++) {
        data.roundData.communityCards.push_back(deck.draw());  // draw flop cards
    }
    return this->betRound(players, data);
}

OutEnum Game::turn(Player* players[], Data& data, Deck& deck, OutEnum lastRes) const noexcept {
    if(lastRes != OutEnum::ROUND_CONTINUE) return lastRes;
    this->setupBetRound(data);
    PLOG_DEBUG << "Starting TURN bet round";
    data.roundData.communityCards.push_back(deck.draw());  // draw turn card
    return this->betRound(players, data);
}

OutEnum Game::river(Player* players[], Data& data, Deck& deck, OutEnum lastRes) const noexcept {
    if(lastRes != OutEnum::ROUND_CONTINUE) return lastRes;
    this->setupBetRound(data);
    PLOG_DEBUG << "Starting RIVER bet round";
    data.roundData.communityCards.push_back(deck.draw());  // draw river card
    return this->betRound(players, data);
}
