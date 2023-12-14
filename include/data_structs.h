#pragma once
#include <iostream>
#include <vector>

enum class Actions{
    FOLD,
    CALL,
    RAISE,
    CHECK,
    BET,
};

struct Action{
    Actions action;
    u_int64_t bet = 0;
};

// contains the data for a single bet round (preflop, flop, turn, river)
struct BetRoundData{
    u_int8_t playerPos; // position of the player that is currently playing
    u_int64_t currentBet;   // current bet
    std::vector<u_int64_t> playerBets; // bets of the players in the current round
};

// contains the data for a single round (until the pot is won)
struct RoundData{
    u_int64_t smallBlind; // small blind
    u_int64_t bigBlind;   // big blind
    u_int8_t dealerPos; // position of the dealer
    u_int64_t pot;      // current pot
    std::vector<bool> playerFolded; // true if player folded
    std::vector<Card> communityCards;   // community cards
};

// contains the data for a single game (until only one player is left)
struct GameData{
    std::vector<bool> playerOut;    // true if player is out of the game
    std::vector<u_int64_t> playerChips; // chips of the players
};

// contains all Data for a game set (multiple games)
struct Data{
    u_int8_t numPlayers;    //number of players that are competing
    GameData gameData;
    RoundData roundData;
    BetRoundData betRoundData;

    u_int8_t getNextPlayerPos(u_int8_t playerPos, const bool consider_folded) const noexcept{
        do{
            playerPos = (playerPos + 1) % this->numPlayers;
        }while(this->gameData.playerOut[playerPos] || (!consider_folded && this->roundData.playerFolded[playerPos]));
        return playerPos;
    }

    void nextPlayer() noexcept{
        do{
            this->betRoundData.playerPos = (this->betRoundData.playerPos + 1) % this->numPlayers;
        }while(this->gameData.playerOut[this->betRoundData.playerPos]);
    };

    void selectDealer(const bool firstRound) noexcept{
        if(firstRound){
            this->roundData.dealerPos = 0;
        }else{
            do{
                this->roundData.dealerPos = (this->roundData.dealerPos + 1) % this->numPlayers;
            }while(this->gameData.playerOut[this->roundData.dealerPos]);
        }
    }

    bool removeChips(u_int64_t chips) noexcept {
        if(this->gameData.playerChips[this->betRoundData.playerPos] < chips)
            return false;
        this->gameData.playerChips[this->betRoundData.playerPos] -= chips;
        return true;
    }

    u_int64_t getChips() const noexcept{
        return this->gameData.playerChips[this->betRoundData.playerPos];
    }
};

struct BetRoundResult{
    bool gameWon = false;           // true if the game was won and only one player is left with chips
    bool potWon = false;            // true if the pot was won and the round is over
};