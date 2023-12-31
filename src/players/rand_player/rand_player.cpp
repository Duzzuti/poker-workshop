#include "rand_player.h"

Action RandPlayer::turn(const Data& data, const bool onlyRaise) const noexcept {
    Action action;
    u_int8_t randMod = 10;
    if (onlyRaise) randMod = 4;
    bool done = false;
    while (!done) {
        switch (std::rand() % randMod) {
            case 0 ... 2:  // Call
                action.action = Actions::CALL;
                if (data.getChips() >= data.getCallAdd() && data.betRoundData.currentBet != 0) done = true;
                break;

            case 3:  // Raise
                action.action = Actions::RAISE;
                action.bet = (u_int64_t)(data.betRoundData.currentBet + data.betRoundData.minimumRaise * (1 + (std::rand() % 30) / 10.0f));
                if (data.getChips() >= data.getRaiseAdd(action.bet) && data.betRoundData.currentBet != 0) done = true;
                break;

            case 4 ... 5:  // Fold
                action.action = Actions::FOLD;
                done = true;
                break;

            case 6 ... 7:  // Check
                action.action = Actions::CHECK;
                if (data.betRoundData.currentBet == 0) done = true;
                break;

            case 8 ... 9:  // Bet
                action.action = Actions::BET;
                action.bet = (u_int64_t)(data.roundData.bigBlind * (1 + (std::rand() % 30) / 10.0f));
                if (data.getChips() >= action.bet && data.betRoundData.currentBet == 0) done = true;
                break;
        }
    }

    return action;
}