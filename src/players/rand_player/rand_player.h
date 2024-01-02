#pragma once
#include "player.h"

class RandPlayer : public Player {
   public:
    RandPlayer(const char* name) : Player(name){};
    RandPlayer(u_int8_t playerNum = 0) : Player(createPlayerName("RandPlayer", playerNum)){};

    Action turn(const Data& data, const bool onlyRaise = false) const noexcept override;
};