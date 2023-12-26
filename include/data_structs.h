#pragma once
#include <iostream>
#include <vector>

#include "config.h"
#include "deck.h"

enum class Actions {
    FOLD,
    CALL,
    RAISE,
    CHECK,
    BET,
};

enum CardSuits {
    DIAMONDS = 0,
    HEARTS,
    SPADES,
    CLUBS,
};

enum HandKinds {
    NO_HAND = 0,
    HIGH_CARD,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH,
};

struct Action {
    Actions action;
    u_int64_t bet = 0;
};

enum class OutEnum {
    ROUND_CONTINUE,
    GAME_WON,
    ROUND_WON,
};

// contains the data for a single bet round (preflop, flop, turn, river)
struct BetRoundData {
    u_int8_t playerPos;                 // position of the player that is currently playing
    u_int64_t currentBet;               // current bet
    u_int64_t playerBets[MAX_PLAYERS];  // bets of the players in the current round
};

// contains the data for a single round (until the pot is won)
struct RoundData {
    u_int64_t smallBlind;            // small blind
    u_int64_t bigBlind;              // big blind
    u_int64_t addBlind;              // add blind amount every time the dealer is again at position 0
    u_int8_t dealerPos;              // position of the dealer
    u_int64_t pot;                   // current pot
    bool playerFolded[MAX_PLAYERS];  // true if player folded
    Card communityCards[5];          // community cards
    OutEnum result;                  // whats the state of the round (continue, round won, game won)
};

// contains the data for a single game (until only one player is left)
struct GameData {
    bool playerOut[MAX_PLAYERS];         // true if player is out of the game
    u_int64_t playerChips[MAX_PLAYERS];  // chips of the players
};

// contains all Data for a game set (multiple games)
struct Data {
    u_int8_t numPlayers;  // number of players that are competing
    GameData gameData;
    RoundData roundData;
    BetRoundData betRoundData;

    u_int8_t getNextPlayerPos(u_int8_t playerPos, const bool consider_folded) const noexcept {
        do {
            playerPos = (playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[playerPos] || (!consider_folded && this->roundData.playerFolded[playerPos]));
        return playerPos;
    }

    void nextPlayer() noexcept {
        do {
            this->betRoundData.playerPos = (this->betRoundData.playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[this->betRoundData.playerPos]);
    };

    void selectDealer(const bool firstRound) noexcept {
        if (firstRound) {
            this->roundData.dealerPos = 0;
        } else {
            do {
                this->roundData.dealerPos = (this->roundData.dealerPos + 1) % this->numPlayers;
            } while (this->gameData.playerOut[this->roundData.dealerPos]);
        }
    }

    bool removeChips(u_int64_t chips) noexcept {
        if (this->gameData.playerChips[this->betRoundData.playerPos] < chips) return false;
        this->gameData.playerChips[this->betRoundData.playerPos] -= chips;
        return true;
    }

    u_int64_t getChips() const noexcept { return this->gameData.playerChips[this->betRoundData.playerPos]; }

    u_int64_t getCallAdd() const noexcept { return this->betRoundData.currentBet - this->betRoundData.playerBets[this->betRoundData.playerPos]; }

    u_int64_t getRaiseAdd(const u_int64_t bet) const noexcept { return bet - this->betRoundData.playerBets[this->betRoundData.playerPos]; }
};