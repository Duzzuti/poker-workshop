#include <gtest/gtest.h>

#include "check_player/check_player.h"
#include "game_test.h"
#include "rand_player/rand_player.h"

TEST(Utils, PerfinitPlayerOrder) {
    GameTest game{Config(100, 100, 100, 100, 100)};
    u_int8_t numPlayer = 100;
    Player** players = new Player*[numPlayer];
    for (u_int8_t i = 0; i < numPlayer; i += 2) {
        players[i] = new RandPlayer("Player " + std::to_string(i));
        players[i + 1] = new CheckPlayer("Player " + std::to_string(i + 1));
    }
    game.setPlayers(players);
    for (u_int64_t iter = 0; iter < 100; iter++) game.initPlayerOrder();
}