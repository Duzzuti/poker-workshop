#pragma once
#include "player.h"

void initPlayerOrder(Player* players[], u_int8_t numPlayer) noexcept;

std::string getPlayerInfo(Player* players[], const Data& data) noexcept;