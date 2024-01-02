#pragma once
#include "player.h"

class HumanPlayer : public Player {
   public:
    HumanPlayer(const char* name) : Player(name){};
    HumanPlayer(u_int8_t playerNum = 0) : Player(!playerNum ? "HumanPlayer" : ("HumanPlayer" + std::to_string(playerNum)).c_str()){};

    Action turn(const Data& data, const bool onlyRaise = false) const noexcept override;
};