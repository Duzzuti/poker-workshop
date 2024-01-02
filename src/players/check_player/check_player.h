#pragma once
#include "player.h"

class CheckPlayer : public Player {
   public:
    CheckPlayer(const char* name) : Player(name){};
    CheckPlayer(u_int8_t playerNum = 0) : Player(!playerNum ? "CheckPlayer" : ("CheckPlayer" + std::to_string(playerNum)).c_str()){};

    Action turn(const Data& data, const bool onlyRaise = false) const noexcept override;
};