#include "check_player.h"

Action CheckPlayer::turn(const Data& data, const bool blindOption) const noexcept {
    Action action;
    // calls the blind option
    if (blindOption) {
        action.action = Actions::CALL;
        return action;
    }
    // checks if possible, otherwise calls if possible, otherwise folds
    action.action = data.betRoundData.currentBet == 0 ? Actions::CHECK : data.getCallAdd() < data.getChips() ? Actions::CALL : Actions::FOLD;
    return action;
}