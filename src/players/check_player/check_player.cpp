#include "check_player.h"

Action CheckPlayer::turn(const TurnData &data) const noexcept{
    Action action;
    action.action = data.currentBet == 0 ? Actions::CHECK : data.currentBet <= getChips() ? Actions::CALL : Actions::FOLD;
    return action;
}