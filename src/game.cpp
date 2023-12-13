#include <algorithm>

#include "game.h"
#include "check_player/check_player.h"
#include "deck.h"
#include "player.h"
#include "utils.h"
#include "logger.h"

void Game::run() const {
    // config players
    Player* players[m_config.numPlayers];
    // init players
    for (u_int8_t i = 0; i < m_config.numPlayers; i++) {
        players[i] = new CheckPlayer();
        players[i]->setChips(m_config.startingChips);
    }

    Data data;
    data.numPlayers = m_config.numPlayers;

    // run for the number of games specified in the config
    for (u_int64_t round = 0; round < m_config.numRounds; round++) {
        // ONE GAME
        // shuffle players
        shufflePlayers(players, m_config.numPlayers);
        data.gameData.playerOut = std::vector<bool>(m_config.numPlayers, false);
        bool firstRound = true;

        while(std::count(data.gameData.playerOut.begin(), data.gameData.playerOut.end(), false) > 1){
            // ONE ROUND
            Deck deck;
            
            this->startRound(players, data, deck, firstRound);
            firstRound = false;

            // PREFLOP
            this->betRound(players, data);
        }
    }

    // delete players, free memory
    for (u_int8_t i = 0; i < m_config.numPlayers; i++) {
        delete players[i];
    }
}

void Game::setBlinds(Player* players[], Data& data) const noexcept {
    // blinds
    data.nextPlayer();
    this->bet(players, data, data.roundData.smallBlind);
    this->bet(players, data, data.roundData.bigBlind);
}

void Game::startRound(Player* players[], Data& data, Deck& deck, const bool firstRound) const noexcept {
    deck.shuffle();

    data.selectDealer(firstRound);

    data.roundData.smallBlind = m_config.smallBlind;
    data.roundData.bigBlind = m_config.bigBlind;
    data.roundData.pot = 0;
    data.roundData.playerFolded = std::vector<bool>(m_config.numPlayers, false);
    data.betRoundData.playerPos = data.roundData.dealerPos;
    data.betRoundData.playerBets = std::vector<u_int64_t>(m_config.numPlayers, 0);
    data.betRoundData.currentBet = 0;

    // deal cards
    for (u_int8_t i = 0; i < m_config.numPlayers; i++) {
        if(data.gameData.playerOut[i])
            continue;
        players[i]->setHand(deck.draw(), deck.draw());
    }

    this->setBlinds(players, data);
}

BetRoundResult Game::betRound(Player* players[], Data& data) const {
    short firstChecker = -1;    // position of the first player that checked
    // this loop will run until all players have either folded, checked or called
    // we can only exit if it is a players turn and he is in the game, has the same bet as the current bet and all players have checked if the bet is 0
    // we need to consider the case where every player folds except one, then the last player wins the pot
    while(data.roundData.playerFolded[data.betRoundData.playerPos] || data.gameData.playerOut[data.betRoundData.playerPos] || 
    data.betRoundData.currentBet != data.betRoundData.playerBets[data.betRoundData.playerPos] || (data.betRoundData.currentBet == 0 && firstChecker != data.betRoundData.playerPos)){

        if(data.roundData.playerFolded[data.betRoundData.playerPos] || data.gameData.playerOut[data.betRoundData.playerPos]){
            // player is out of the game or folded, skip turn
            data.nextPlayer();
            continue;
        }
        Action action = players[data.betRoundData.playerPos]->turn(data);
        switch (action.action) {
            case Actions::FOLD:
                data.roundData.playerFolded[data.betRoundData.playerPos] = true;
                data.nextPlayer();
                break;
            
            case Actions::CHECK:
                if(data.betRoundData.currentBet != 0){
                    // illegal move leads to loss of the game
                    if(playerOut(players, data))
                        return BetRoundResult{.gameWon = true};
                }else{
                    if(firstChecker == -1)
                        firstChecker = data.betRoundData.playerPos;
                    data.nextPlayer();
                }
                break;
            
            case Actions::CALL:
                if(!this->bet(players, data, data.betRoundData.currentBet)){
                    // illegal move leads to loss of the game
                    if(playerOut(players, data))
                        return BetRoundResult{.gameWon = true};
                }
                break;
            
            case Actions::RAISE:
                if(!this->bet(players, data, action.bet)){
                    // illegal move leads to loss of the game
                    if(playerOut(players, data))
                        return BetRoundResult{.gameWon = true};
                }
                break;

            case Actions::BET:
                if(data.betRoundData.currentBet != 0 || !this->bet(players, data, action.bet)){
                    // illegal move leads to loss of the game
                    if(playerOut(players, data))
                        return BetRoundResult{.gameWon = true};
                }
                break;

            default:
                throw std::logic_error("Invalid action");
                PLOG_FATAL << "Invalid action: " << static_cast<int>(action.action);
        }
    }

    // TODO bet round result
    return BetRoundResult{};
}

bool Game::bet(Player* players[], Data& data, const u_int64_t amount) const noexcept {
    // amount is the whole bet, not the amount that is added to the pot
    if(amount < data.betRoundData.currentBet ||     // call condition
        amount > data.betRoundData.currentBet && amount < data.betRoundData.currentBet*2 || // raise condition
        amount < data.roundData.smallBlind // bet condition
    )
        return false;
    u_int64_t addAmount = amount - data.betRoundData.playerBets[data.betRoundData.playerPos];
    bool success = players[data.betRoundData.playerPos]->removeChips(addAmount);
    if(!success) 
        return success;
    data.betRoundData.currentBet = amount;
    data.roundData.pot += addAmount;
    data.betRoundData.playerBets[data.betRoundData.playerPos] = amount;
    data.nextPlayer();
    return true;
}

bool Game::playerOut(Player* players[], Data& data) const noexcept {
    data.gameData.playerOut[data.betRoundData.playerPos] = true;
    data.nextPlayer();
    // if only one player is left, he wins the game
    return (std::count(data.gameData.playerOut.begin(), data.gameData.playerOut.end(), false) == 1);
}
