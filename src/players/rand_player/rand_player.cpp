#include "rand_player.h"

Action RandPlayer::turn(const Data& data, const bool onlyRaise) const noexcept {
    Action action;
    // control the possible actions with randMod
    u_int8_t randMod = 10;
    // if onlyRaise is true, the player can only raise or call
    if (onlyRaise) randMod = 4;
    // choose random actions until a valid one is found
    bool done = false;
    while (!done) {
        switch (std::rand() % randMod) {
            case 0 ... 2:  // Call
                // if the player has enough chips to call and the bet round is open, call
                action.action = Actions::CALL;
                if (data.getChips() > data.getCallAdd() && data.betRoundData.currentBet != 0) done = true;
                break;

            case 3:  // Raise
                // if the player has enough chips to raise and the bet round is open, raise
                action.action = Actions::RAISE;
                // the bet is a random number between the minimum raise + current bet and the current bet + 4 * minimum raise
                action.bet = (u_int64_t)(data.betRoundData.currentBet + data.betRoundData.minimumRaise * (1 + (std::rand() % 30) / 10.0f));
                if (data.getChips() > data.getRaiseAdd(action.bet) && data.betRoundData.currentBet != 0) done = true;
                break;

            case 4 ... 5:  // Fold
                // fold is always possible
                action.action = Actions::FOLD;
                done = true;
                break;

            case 6 ... 7:  // Check
                // check is only possible if the current bet is 0
                action.action = Actions::CHECK;
                if (data.betRoundData.currentBet == 0) done = true;
                break;

            case 8 ... 9:  // Bet
                // if the player has enough chips to bet and the bet round is not open, bet
                action.action = Actions::BET;
                action.bet = (u_int64_t)(data.roundData.bigBlind * (1 + (std::rand() % 30) / 10.0f));
                if (data.getChips() > action.bet && data.betRoundData.currentBet == 0) done = true;
                break;
        }
    }

    return action;
}