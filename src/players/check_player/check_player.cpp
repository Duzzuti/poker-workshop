#include "check_player.h"

Action CheckPlayer::turn(const Data& data, const bool onlyRaise) const noexcept {
    Action action;
    // calls if the player can only raise or call (match the bet, means bet 0 because the bet is already matched)
    if (onlyRaise) {
        action.action = Actions::CALL;
        return action;
    }
    // checks if possible, otherwise calls if possible, otherwise folds
    action.action = data.betRoundData.currentBet == 0 ? Actions::CHECK : data.getCallAdd() < data.getChips() ? Actions::CALL : Actions::FOLD;
    return action;
}