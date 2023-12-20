#pragma once
#include <iostream>

class Config {
public:
    u_int64_t startingChips;        // Starting chips for each player
    u_int64_t smallBlind;           // Small blind amount
    u_int64_t addBlindPerDealer0;   // Add blind amount everytime the dealer is again at position 0
    u_int64_t numRounds;            // Number of rounds to play
    u_int8_t numPlayers;            // Number of players in the game
    
    // Constructor
    Config(u_int16_t rounds, u_int8_t players, u_int64_t chips, u_int64_t small, u_int64_t addBlind) noexcept
        : startingChips(chips), smallBlind(small), addBlindPerDealer0(addBlind), numRounds(rounds) ,numPlayers(players){}
};

class BaseConfig : public Config {
public:
    BaseConfig() noexcept
        : Config(10, 4, 1000, 10, 1) {} // Default values
};
