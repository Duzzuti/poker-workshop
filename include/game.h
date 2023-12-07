#pragma once
#include "config.h"

class Game {
public:
    Game(const Config& config) noexcept
        : m_config(config) {}

    void run() noexcept;

private:
    Config m_config;
};
