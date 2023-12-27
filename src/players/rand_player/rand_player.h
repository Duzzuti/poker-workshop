#pragma once
#include "player.h"

class RandPlayer : public Player {
   public:
    RandPlayer(const std::string& name) noexcept : Player(name){};
    RandPlayer(u_int8_t playerNum = 0) noexcept : Player(!playerNum ? "RandPlayer" : "RandPlayer" + std::to_string(playerNum)){};

    Action turn(const Data& data) const noexcept override;
};