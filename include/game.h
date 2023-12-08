#pragma once
#include "config.h"
#include "player.h"

class Game {
public:
    Game(const Config& config) noexcept
        : m_config(config) {}

    void run() const;

private:

    void setBlinds(Player* players[], Data& turnData) const noexcept;
    void startRound(Player* players[], Data& turnData, Deck& deck, const bool firstRound) const noexcept;
    BetRoundResult betRound(Player* players[], Data& turnData) const;
    bool bet(Player* players[], Data& turnData, const u_int64_t amount) const noexcept;

    Config m_config;
};
