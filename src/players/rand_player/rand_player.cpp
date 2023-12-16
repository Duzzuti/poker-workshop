#include "rand_player.h"

Action RandPlayer::turn(const Data &data) const noexcept{
    Action action;
    bool done = false;
    while(!done){
        switch (this->dist10(this->rng)){
            case 0 ... 1: // Fold
                action.action = Actions::FOLD;
                done = true;
                break;
            case 2 ... 3: // Check
                action.action = Actions::CHECK;
                if(data.betRoundData.currentBet == 0)
                    done = true;
                break;
            
            case 4 ... 6: // Call
                action.action = Actions::CALL;
                if(data.getChips() >= data.getCallAdd() && data.betRoundData.currentBet != 0)
                    done = true;
                break;
            
            case 7: // Raise
                action.action = Actions::RAISE;
                action.bet = (u_int64_t) (data.betRoundData.currentBet * (2 + (rand() % 20)/10.0f));
                if(data.getChips() >= data.getRaiseAdd(action.bet) && data.betRoundData.currentBet != 0)
                    done = true;
                break;
            
            case 8 ... 9: // Bet
                action.action = Actions::BET;
                action.bet = (u_int64_t) (data.roundData.smallBlind * (1 + (rand() % 30)/10.0f));
                if(data.getChips() >= action.bet && data.betRoundData.currentBet == 0)
                    done = true;
                break;
        }
    }
    
    return action;
}