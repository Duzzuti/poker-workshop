#pragma once

#include "player.h"

class HandStrengths{
public:

    HandKinds handkind;
    u_int32_t rankStrength;

    HandStrengths(HandKinds handkind, u_int32_t rankStrength) noexcept
        : handkind(handkind), rankStrength(rankStrength) {};

    static std::vector<HandStrengths> getHandStrengths(Player* players[], const Data& data) noexcept;
    static u_int32_t getRankStrength(const std::vector<Card> sortedCards, const u_int8_t num) noexcept;

};