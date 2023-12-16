#include "check_player.h"

Action CheckPlayer::turn(const Data &data) const noexcept{
    Action action;
    action.action = data.betRoundData.currentBet == 0 ? Actions::CHECK : data.getCallAdd() <= data.getChips() ? Actions::CALL : Actions::FOLD;
    return action;
}