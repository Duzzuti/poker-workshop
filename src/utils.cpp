#include "utils.h"
#include "logger.h"

void initPlayerOrder(Player* players[], u_int8_t numPlayer) noexcept {
    // shuffle deck
    std::random_shuffle(&players[0], &players[numPlayer]);
    PLOG_INFO << "Shuffled players, new order:";
    for (u_int8_t i = 0; i < numPlayer; i++) {
        players[i]->setPlayerPosNum(i);
        PLOG_INFO << players[i]->getName();
    }
}

std::string getPlayerInfo(Player* players[], const Data& data) noexcept { return "Player " + players[data.betRoundData.playerPos]->getName() + "[" + std::to_string(data.getChips()) + "]"; }