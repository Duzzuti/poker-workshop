#include "utils.h"

#include <gtest/gtest.h>

#include "check_player/check_player.h"
#include "rand_player/rand_player.h"

TEST(Utils, PerfinitPlayerOrder) {
    u_int8_t numPlayer = 100;
    Player* players[numPlayer];
    for (u_int8_t i = 0; i < numPlayer; i += 2) {
        players[i] = new RandPlayer("Player " + std::to_string(i));
        players[i + 1] = new CheckPlayer("Player " + std::to_string(i + 1));
    }
    for (u_int64_t iter = 0; iter < 1000; iter++) initPlayerOrder(players, numPlayer);

    // delete players, free memory
    for (u_int8_t i = 0; i < numPlayer; i++) {
        delete players[i];
    }
}