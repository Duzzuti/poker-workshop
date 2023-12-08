#include "player.h"

class CheckPlayer : public Player {
public:
    CheckPlayer() noexcept : Player("CheckPlayer") {};

    Action turn(const Data& data) const noexcept override;
};