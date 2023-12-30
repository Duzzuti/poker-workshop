#pragma once
#include <iostream>

#include "logger.h"

const constexpr u_int8_t MAX_PLAYERS = 10;

class Config {
   public:
    u_int64_t startingChips;       // Starting chips for each player
    u_int64_t smallBlind;          // Small blind amount
    u_int64_t addBlindPerDealer0;  // Add blind amount everytime the dealer is again at position 0
    u_int64_t numRounds;           // Number of rounds to play
    u_int8_t numPlayers;           // Number of players in the game

    // Constructor
    Config(u_int16_t rounds, u_int8_t players, u_int64_t chips, u_int64_t small, u_int64_t addBlind)
        : startingChips(chips), smallBlind(small), addBlindPerDealer0(addBlind), numRounds(rounds), numPlayers(players) {
        if (this->numPlayers < 2 || this->numPlayers > MAX_PLAYERS) {
            PLOG_FATAL << "Invalid number of players: " << this->numPlayers << " (min: 2, max: " << MAX_PLAYERS << ")";
            throw std::invalid_argument("Invalid number of players");
        }
    }
};

class BaseConfig : public Config {
   public:
    BaseConfig() : Config(10, 5, 1000, 10, 1) {}  // Default values
};
