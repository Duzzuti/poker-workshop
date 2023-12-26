#pragma once

#include "player.h"

class HandStrengths {
   public:
    HandKinds handkind;
    u_int32_t rankStrength;

    HandStrengths(HandKinds handkind, u_int32_t rankStrength) noexcept : handkind(handkind), rankStrength(rankStrength){};
    HandStrengths() noexcept : handkind(HandKinds::NO_HAND), rankStrength(0){};

    static void getHandStrengths(Player* const players[], const Data& data, HandStrengths result[]) noexcept;
    static HandStrengths getHandStrength(const std::pair<Card, Card>& hand, const Card community[]) noexcept;
    static u_int32_t getRankStrength(const Card sortedCards[], const u_int8_t num) noexcept;

    friend bool operator>(const HandStrengths& lhs, const HandStrengths& rhs) noexcept;
    friend bool operator==(const HandStrengths& lhs, const HandStrengths& rhs) noexcept;
};