#include "human_player.h"

Action HumanPlayer::turn(const Data& data, const bool blindOption, const bool equalize) const noexcept {
    // prints the current state of the game
    std::cout << "Your hand: " << this->getHand().first.toString() << " " << this->getHand().second.toString();
    // prints the community cards
    std::cout << " | Community cards: ";
    for (u_int8_t i = 0; i < 5; i++) {
        if (data.roundData.betRoundState == BetRoundState::PREFLOP)
            break;
        else if (data.roundData.betRoundState == BetRoundState::FLOP && i == 3)
            break;
        else if (data.roundData.betRoundState == BetRoundState::TURN && i == 4)
            break;

        std::cout << data.roundData.communityCards[i].toString() << " ";
    }
    // prints the pot, current bet, your chips and the minimum raise/bet
    std::cout << "| Pot: " << data.roundData.pot << " | Current bet: " << data.betRoundData.currentBet;
    std::cout << " | Your chips: " << data.getChips() << " | Minimum raise/bet: " << data.betRoundData.minimumRaise + data.betRoundData.currentBet << std::endl;
    while (true) {
        // asks the user what to do
        // if the user is the has the blind option, the user can only call, raise or all-in
        if (blindOption)
            std::cout << "Please enter an action ('c' for call, 'r <bet>' for raise, 'a' for all-in): ";
        else if (equalize && data.getCallAdd() < data.getChips())
            std::cout << "Please enter an action ('c' for call (equalize), 'f' for fold): ";
        else if (equalize)
            std::cout << "Please enter an action ('c' for call (equalize), 'f' for fold, 'a' for all-in): ";
        else
            std::cout << "Please enter an action ('f' for fold, 'c' for call, 'chk' for check, 'r <bet>' for raise, 'b <bet>' for bet, 'a' for all-in): ";
        std::string input;
        std::getline(std::cin, input);

        // checks the input and returns the corresponding action
        if (input == "f" && !blindOption)
            return {Actions::FOLD};
        else if (input == "c")
            return {Actions::CALL};
        else if (input == "chk" && !blindOption && !equalize)
            return {Actions::CHECK};
        else if (input == "a" && !(equalize && data.getCallAdd() < data.getChips()))
            return {Actions::ALL_IN};
        else if (input[0] == 'r' && !equalize) {
            try {
                u_int64_t bet = std::stoull(input.substr(2));
                return {Actions::RAISE, bet};
            } catch (std::invalid_argument& e) {
                std::cout << "Invalid input! Try again" << std::endl;
                continue;
            }
        } else if (input[0] == 'b' && !blindOption && !equalize) {
            try {
                u_int64_t bet = std::stoull(input.substr(2));
                return {Actions::BET, bet};
            } catch (std::invalid_argument& e) {
                std::cout << "Invalid input! Try again" << std::endl;
                continue;
            }
        } else {
            std::cout << "Invalid input! Try again" << std::endl;
        }
    }
}