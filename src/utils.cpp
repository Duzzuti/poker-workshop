#include "utils.h"

#include <random>

#include "logger.h"

void shufflePlayers(Player* players[], u_int8_t numPlayer) noexcept {
    // shuffle deck
    std::random_device dev;
    std::mt19937 rng{dev()};
    std::uniform_int_distribution<std::mt19937::result_type> dist{0, INT32_MAX};  // distribution in max u_int32 range

    for (u_int8_t i = numPlayer - 1; i > 0; --i) {
        u_int8_t j = dist(rng) % (i + 1);  // Generate a random index within the unshuffled part of the array

        // Swap characters at indices i and j
        Player* temp = players[i];
        players[i] = players[j];
        players[j] = temp;
    }
    PLOG_INFO << "Shuffled players, new order:";
    for (u_int8_t i = 0; i < numPlayer; i++) {
        players[i]->setPlayerPosNum(i);
        PLOG_INFO << players[i]->getName();
    }
}

std::string getPlayerInfo(Player* players[], const Data& data) noexcept { return "Player " + players[data.betRoundData.playerPos]->getName() + "[" + std::to_string(data.getChips()) + "]"; }