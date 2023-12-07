#pragma once
#include <iostream>

class Config {
public:
    u_int64_t startingChips;    // Starting chips for each player
    u_int64_t smallBlind;       // Small blind amount
    u_int64_t bigBlind;         // Big blind amount
    u_int64_t numRounds;        // Number of rounds to play
    u_int8_t numPlayers;        // Number of players in the game
    
    // Constructor
    Config(u_int16_t rounds, u_int8_t players, u_int64_t chips, u_int64_t small, u_int64_t big) noexcept
        : numRounds(rounds) ,numPlayers(players), startingChips(chips), smallBlind(small), bigBlind(big) {}
};

class BaseConfig : public Config {
public:
    BaseConfig() noexcept
        : Config(10, 4, 1000, 10, 20) {} // Default values
};
