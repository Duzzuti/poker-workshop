#include "rand_player.h"

Action RandPlayer::turn(const Data &data) const noexcept{
    srand(static_cast<unsigned int>(time(nullptr))); // Seed for randomization
    Action action;
    bool done = false;
    while(!done){
        switch (rand() % 10){
            case 0 ... 1: // Fold
                action.action = Actions::FOLD;
                done = true;
                break;
            case 2 ... 4: // Check
                action.action = Actions::CHECK;
                if(data.betRoundData.currentBet == 0)
                    done = true;
                break;
            
            case 5 ... 7: // Call
                action.action = Actions::CALL;
                if(data.getChips() >= data.betRoundData.currentBet && data.betRoundData.currentBet != 0)
                    done = true;
                break;
            
            case 8: // Raise
                action.action = Actions::RAISE;
                action.bet = (u_int64_t) (data.betRoundData.currentBet * (2 + (rand() % 20)/10.0f));
                if(data.getChips() >= action.bet && data.betRoundData.currentBet != 0)
                    done = true;
                break;
            
            case 9 ... 10: // Bet
                action.action = Actions::BET;
                action.bet = (u_int64_t) (data.roundData.smallBlind * (1 + (rand() % 30)/10.0f));
                if(data.getChips() >= action.bet && data.betRoundData.currentBet == 0)
                    done = true;
                break;
        }
    }
    
    return action;
}