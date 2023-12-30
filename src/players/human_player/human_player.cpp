#include "human_player.h"

Action HumanPlayer::turn(const Data& data, const bool onlyRaise) const noexcept {
    std::cout << "Your hand: " << this->getHand().first.toString() << " " << this->getHand().second.toString();
    std::cout << " | Community cards: ";
    for(u_int8_t i = 0; i < 5; i++) {
        if(data.roundData.betRoundState == BetRoundState::PREFLOP)
            break;
        else if(data.roundData.betRoundState == BetRoundState::FLOP && i == 3)
            break;
        else if(data.roundData.betRoundState == BetRoundState::TURN && i == 4)
            break;
        
        std::cout << data.roundData.communityCards[i].toString() << " ";
    }
    std::cout << "| Pot: " << data.roundData.pot << " | Current bet: " << data.betRoundData.currentBet;
    std::cout << " | Your chips: " << data.getChips() << std::endl;
    while(true){
        if(onlyRaise)
            std::cout << "Please enter an action ('c' for call, 'r <bet>' for raise): ";
        else
            std::cout << "Please enter an action ('f' for fold, 'c' for call, 'chk' for check, 'r <bet>' for raise, 'b <bet>' for bet): ";
        std::string input;
        std::getline(std::cin, input);

        if(input == "f" && !onlyRaise)
            return {Actions::FOLD};
        else if(input == "c")
            return {Actions::CALL};
        else if(input == "chk" && !onlyRaise)
            return {Actions::CHECK};
        else if(input[0] == 'r') {
            u_int64_t bet = std::stoull(input.substr(2));
            return {Actions::RAISE, bet};
        } else if(input[0] == 'b' && !onlyRaise) {
            u_int64_t bet = std::stoull(input.substr(2));
            return {Actions::BET, bet};
        } else {
            std::cout << "Invalid input! Try again" << std::endl;
        }
    }
}