#pragma once
#include "player.h"

void shufflePlayers(Player* players[], u_int8_t numPlayer) noexcept;

std::string getPlayerInfo(Player* players[], const Data& data) noexcept;