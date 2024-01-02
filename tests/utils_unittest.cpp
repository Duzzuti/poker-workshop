#include <gtest/gtest.h>

#include "game_test.h"

TEST(Utils, PerfinitPlayerOrder) {
    GameTest game{Config(100, 10, 100, 100, 100)};
    u_int8_t numPlayer = 10;
    std::unique_ptr<Player>* players = new std::unique_ptr<Player>[numPlayer];
    for (u_int8_t i = 0; i < numPlayer; i += 2) {
        players[i] = std::move(std::make_unique<RandPlayer>(("Player " + std::to_string(i)).c_str()));
        players[i + 1] = std::move(std::make_unique<CheckPlayer>(("Player " + std::to_string(i + 1)).c_str()));
    }
    game.setPlayers(std::move(players));
    for (u_int64_t iter = 0; iter < 100; iter++) game.initPlayerOrder();
}