#pragma once
#include "player.h"

class RandPlayer : public Player {
   public:
    RandPlayer(const std::string& name) : Player(name){};
    RandPlayer(u_int8_t playerNum = 0) : Player(!playerNum ? "RandPlayer" : "RandPlayer" + std::to_string(playerNum)){};

    Action turn(const Data& data, const bool onlyRaise = false) const noexcept override;
};