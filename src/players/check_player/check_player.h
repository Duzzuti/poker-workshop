#include "player.h"

class CheckPlayer : public Player {
public:
    CheckPlayer() noexcept : Player("CheckPlayer") {};

    Action turn(const TurnData& data) const noexcept override;
};