#pragma once
#include "player.h"

class HumanPlayer : public Player {
   public:
    HumanPlayer(const char* name) : Player(name){};
    HumanPlayer(u_int8_t playerNum = 0) : Player(createPlayerName("HumanPlayer", playerNum)){};

    Action turn(const Data& data, const bool onlyRaise = false) const noexcept override;
};