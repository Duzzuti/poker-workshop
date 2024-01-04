#include "check_player.h"

Action CheckPlayer::turn(const Data& data, const bool onlyRaise) const noexcept {
    Action action;
    if (onlyRaise) {
        action.action = Actions::CALL;
        return action;
    }
    action.action = data.betRoundData.currentBet == 0 ? Actions::CHECK : data.getCallAdd() < data.getChips() ? Actions::CALL : Actions::FOLD;
    return action;
}