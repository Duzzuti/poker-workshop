#include <algorithm>

#include "game.h"
#include "check_player/check_player.h"
#include "rand_player/rand_player.h"
#include "deck.h"
#include "player.h"
#include "utils.h"
#include "logger.h"

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
        shufflePlayers(players, m_config.numPlayers);
        data.gameData.playerOut = std::vector<bool>(m_config.numPlayers, false);
        data.gameData.playerChips = std::vector<u_int64_t>(m_config.numPlayers, m_config.startingChips);
        bool firstRound = true;
        int32_t round = -1;

        while(std::count(data.gameData.playerOut.begin(), data.gameData.playerOut.end(), false) > 1){
            // ONE ROUND
            round++;
            Deck deck;
            
            PLOG_DEBUG << "Starting round " << round;
            this->startRound(players, data, deck, firstRound);
            firstRound = false;

            // PREFLOP
            PLOG_DEBUG << "Starting PREFLOP bet round";
            this->betRound(players, data);

            // FLOP
            this->setupBetRound(data);
            PLOG_DEBUG << "Starting FLOP bet round";
            for (u_int8_t i = 0; i < 3; i++) {
                data.roundData.communityCards.push_back(deck.draw());    // draw flop card
            }
            this->betRound(players, data);

            // TURN
            this->setupBetRound(data);
            PLOG_DEBUG << "Starting TURN bet round";
            data.roundData.communityCards.push_back(deck.draw());    // draw turn card
            this->betRound(players, data);

            // RIVER
            this->setupBetRound(data);
            PLOG_DEBUG << "Starting RIVER bet round";
            data.roundData.communityCards.push_back(deck.draw());    // draw river card
            this->betRound(players, data);

            // SHOWDOWN
            PLOG_DEBUG << "Run till SHOWDOWN";
            // TODO
        }
    }

    // delete players, free memory
    for (u_int8_t i = 0; i < m_config.numPlayers; i++) {
        delete players[i];
    }
}

void Game::setBlinds(Player* players[], Data& data) const noexcept {
    // blinds
    PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " bets small blind " << data.roundData.smallBlind;
    this->bet(data, data.roundData.smallBlind);
    PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " bets big blind " << data.roundData.bigBlind;
    this->bet(data, data.roundData.bigBlind);
}

void Game::setupBetRound(Data& data) const noexcept {
    data.betRoundData.playerPos = data.roundData.dealerPos;
    data.nextPlayer();
    data.betRoundData.playerBets = std::vector<u_int64_t>(m_config.numPlayers, 0);
    data.betRoundData.currentBet = 0;
}

void Game::startRound(Player* players[], Data& data, Deck& deck, const bool firstRound) const noexcept {
    deck.shuffle();

    data.selectDealer(firstRound);

    data.roundData.smallBlind = m_config.smallBlind;
    data.roundData.bigBlind = m_config.bigBlind;
    data.roundData.pot = 0;
    data.roundData.playerFolded = std::vector<bool>(m_config.numPlayers, false);
    data.roundData.communityCards = std::vector<Card>();
    this->setupBetRound(data);

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
        if(this->playerTurn(players, data, &firstChecker).gameWon)
            return BetRoundResult{.gameWon = true};
    }

    PLOG_DEBUG << "Bet round finished with bet " << data.betRoundData.currentBet << " and pot " << data.roundData.pot;

    // TODO bet round result
    return BetRoundResult{};
}

BetRoundResult Game::playerTurn(Player* players[], Data& data, short* firstChecker) const {

    Action action = players[data.betRoundData.playerPos]->turn(data);
    switch (action.action) {
        case Actions::FOLD:
            PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " folded";
            data.roundData.playerFolded[data.betRoundData.playerPos] = true;
            data.nextPlayer();
            break;
        
        case Actions::CHECK:
            PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " checked";
            if(data.betRoundData.currentBet != 0){
                // illegal move leads to loss of the game
                if(playerOut(players, data))
                    return BetRoundResult{.gameWon = true};
            }else{
                if(*firstChecker == -1)
                    *firstChecker = data.betRoundData.playerPos;
                data.nextPlayer();
            }
            break;
        
        case Actions::CALL:
            PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " called";
            if(!this->bet(data, data.betRoundData.currentBet)){
                // illegal move leads to loss of the game
                if(playerOut(players, data))
                    return BetRoundResult{.gameWon = true};
            }
            break;
        
        case Actions::RAISE:
            PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " raised to " << action.bet;
            if(!this->bet(data, action.bet)){
                // illegal move leads to loss of the game
                if(playerOut(players, data))
                    return BetRoundResult{.gameWon = true};
            }
            break;

        case Actions::BET:
            PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " bet " << action.bet;
            if(data.betRoundData.currentBet != 0 || !this->bet(data, action.bet)){
                // illegal move leads to loss of the game
                if(playerOut(players, data))
                    return BetRoundResult{.gameWon = true};
            }
            break;

        default:
            throw std::logic_error("Invalid action");
            PLOG_FATAL << "Invalid action: " << static_cast<int>(action.action);
    }
    return BetRoundResult{};
}

bool Game::bet(Data& data, const u_int64_t amount) const noexcept {
    // amount is the whole bet, not the amount that is added to the pot
    if(amount < data.betRoundData.currentBet ||     // call condition
        amount > data.betRoundData.currentBet && amount < data.betRoundData.currentBet*2 || // raise condition
        amount < data.roundData.smallBlind // bet condition
    )
        return false;
    u_int64_t addAmount = amount - data.betRoundData.playerBets[data.betRoundData.playerPos];
    bool success = data.removeChips(addAmount);
    if(!success) 
        return success;
    data.betRoundData.currentBet = amount;
    data.roundData.pot += addAmount;
    data.betRoundData.playerBets[data.betRoundData.playerPos] = amount;
    data.nextPlayer();
    return true;
}

bool Game::playerOut(Player* players[], Data& data) const noexcept {
    PLOG_DEBUG << "Player " << players[data.betRoundData.playerPos]->getName() << " is out";
    data.gameData.playerOut[data.betRoundData.playerPos] = true;
    data.nextPlayer();
    // if only one player is left, he wins the game
    return (std::count(data.gameData.playerOut.begin(), data.gameData.playerOut.end(), false) == 1);
}
