#pragma once
#include "config.h"
#include "player.h"

class Game {
public:
    Game(const Config& config) noexcept
        : m_config(config) {}

    void run() const noexcept;

private:

    void setBlinds(Player* players[], TurnData& turnData) const noexcept;
    void startRound(Player* players[], TurnData& turnData, Deck& deck, const u_int8_t dealerPos) const noexcept;

    Config m_config;
};
