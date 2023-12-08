#pragma once
#include <iostream>

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

struct PlayerTurn{
    u_int8_t playerPos;
    u_int8_t numPlayers;

    void operator++(int) noexcept{
        playerPos = (playerPos + 1) % numPlayers;
    }
};

struct TurnData{
    u_int64_t pot;
    u_int64_t currentBet;
    u_int8_t dealerPos;

    PlayerTurn pos;
};