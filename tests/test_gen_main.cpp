#include <iostream>
#include <optional>

#include "config.h"
#include "game_test.h"

/// @brief Get an unsigned long from the user
/// @param askForInput The string to ask the user for input
/// @return The unsigned long from the user
/// @exception Guarantee No-throw
u_int64_t getUL(const std::string askForInput) noexcept {
    std::cout << askForInput;

    u_int64_t inputNum;
    bool validInput = false;
    while (!validInput) {
        std::string userInput;
        std::getline(std::cin, userInput);
        try {
            inputNum = std::stoull(userInput);
            validInput = true;
        } catch (std::exception& e) {
            std::cout << "Invalid input. Please enter a number: ";
        }
    }
    return inputNum;
}

/// @brief Get an unsigned char from the user
/// @param askForInput The string to ask the user for input
/// @return The unsigned char from the user
/// @exception Guarantee No-throw
u_int8_t getUC(const std::string askForInput) noexcept {
    std::cout << askForInput;

    int32_t inputNum;
    bool validInput = false;
    while (!validInput) {
        std::string userInput;
        std::getline(std::cin, userInput);
        try {
            inputNum = std::stoi(userInput);
            if (inputNum < 0 || inputNum > 255) {
                std::cout << "Invalid input. Please enter a number between 0 and 255: ";
                continue;
            }
            validInput = true;
        } catch (std::exception& e) {
            std::cout << "Invalid input. Please enter a number: ";
        }
    }
    return (u_int8_t)inputNum;
}

/// @brief Get a card from the user
/// @param askForInput The string to ask the user for input
/// @return The card from the user
/// @exception Guarantee No-throw
/// @note The input should be in the format <H/D/C/S><2-9/T/J/Q/K/A>
Card getCard(const std::string askForInput) noexcept {
    std::cout << askForInput;
    while (true) {
        std::string userInput;
        std::getline(std::cin, userInput);
        try {
            userInput[0] = std::toupper(userInput[0]);
            userInput[1] = std::toupper(userInput[1]);
            // get the rank, either a single digit or a letter
            u_int8_t rank = (userInput[1] - '0');
            if (userInput[1] == 'T')
                rank = 10;
            else if (userInput[1] == 'J')
                rank = 11;
            else if (userInput[1] == 'Q')
                rank = 12;
            else if (userInput[1] == 'K')
                rank = 13;
            else if (userInput[1] == 'A')
                rank = 14;
            // check if the rank is valid
            if (rank < 2 || rank > 14) throw std::invalid_argument("Invalid rank");
            switch (userInput[0]) {
                case 'H':
                    return Card{.rank = rank, .suit = 1};
                case 'D':
                    return Card{.rank = rank, .suit = 0};
                case 'C':
                    return Card{.rank = rank, .suit = 3};
                case 'S':
                    return Card{.rank = rank, .suit = 2};
                default:
                    // invalid suit entered
                    throw std::invalid_argument("Invalid suit");
            }
        } catch (std::exception& e) {
            std::cout << "Invalid input. Please enter a valid card in the format <H/D/C/S><2-9/T/J/Q/K/A>: ";
        }
    }
}

/// @brief Ask the user for the amount of chips for each player
/// @param numPlayers The number of players in the game
/// @param playerChips The array to store the amount of chips for each player
/// @exception Guarantee No-throw
void getPlayersChips(const u_int8_t numPlayers, u_int64_t playerChips[]) noexcept {
    for (u_int8_t i = 0; i < numPlayers; i++) playerChips[i] = getUL("Enter the amount of chips for player " + std::to_string(i + 1) + ": ");
}

/// @brief Gets one move for a player from the user
/// @param playerInd The index of the player making the move
/// @param first If it is the first move of the round
/// @param bigBlind The amount of the big blind
/// @return The action of the player or an empty optional if the bet round is over
/// @exception Guarantee No-throw
/// @note The input should be in the format <action> [<bet>]
std::optional<Action> getMove(const int16_t playerInd, const bool first, const u_int64_t bigBlind) noexcept {
    while (true) {
        if (first)
            std::cout << "Player " << playerInd << " turn after big blind (" << bigBlind << ") was set" << std::endl;
        else
            std::cout << "Player " << playerInd << " turn" << std::endl;
        std::cout << "Please enter an action ('f' for fold, 'c' for call, 'chk' for check, 'r <bet>' for raise, 'b <bet>' for bet) or 'next' if the bet round is over: ";
        std::string input;
        std::getline(std::cin, input);

        // bet round is over
        if (input == "next") return {};

        // checks the input and returns the corresponding action
        if (input == "f")
            return Action{Actions::FOLD};
        else if (input == "c")
            return Action{Actions::CALL};
        else if (input == "chk")
            return Action{Actions::CHECK};
        else if (input[0] == 'r') {
            try {
                u_int64_t bet = std::stoull(input.substr(2));
                return Action{Actions::RAISE, bet};
            } catch (std::invalid_argument& e) {
                std::cout << "Invalid input! Try again" << std::endl;
                continue;
            }
        } else if (input[0] == 'b') {
            try {
                u_int64_t bet = std::stoull(input.substr(2));
                return Action{Actions::BET, bet};
            } catch (std::invalid_argument& e) {
                std::cout << "Invalid input! Try again" << std::endl;
                continue;
            }
        } else {
            std::cout << "Invalid input! Try again" << std::endl;
        }
    }
}

/// @brief Gets the moves and community cards for each player in the game from the user
/// @param numPlayers The number of players in the game
/// @param bigBlind The amount of the big blind
/// @param firstPlayer The index of the first player to make a move (after the big blind)
/// @exception Guarantee Strong
/// @throw std::invalid_argument If the gotten action or the bet round state is invalid
/// @note The input is not verified, because we want to test the game with that input
/// @note The input is not processed (players who fold are not removed from the game, etc.)
/// @note The user is informed about all previous actions before asking for the next action
void getMoves(const u_int8_t numPlayers, const u_int64_t bigBlind, u_int8_t firstPlayer = 2) {
    BetRoundState betRoundState = BetRoundState::PREFLOP;
    bool first = true;
    // store the information about the moves and community cards
    std::stringstream turns;
    Card communityCards[5];
    while (true) {
        int16_t playerInd = firstPlayer - 1;
        while (true) {
            playerInd++;
            if (playerInd == numPlayers) {
                playerInd = 0;
                turns << std::endl;
            }
            // get the move of the player
            std::optional<Action> move = getMove(playerInd, first, bigBlind);
            first = false;
            // bet round is over
            if (!move.has_value()) {
                turns << "Bet round is over" << std::endl;
                break;
            }
            switch (move.value().action) {
                case Actions::FOLD:
                    turns << "Player " << playerInd << " folded" << std::endl;
                    break;
                case Actions::CALL:
                    turns << "Player " << playerInd << " called" << std::endl;
                    break;
                case Actions::CHECK:
                    turns << "Player " << playerInd << " checked" << std::endl;
                    break;
                case Actions::RAISE:
                    turns << "Player " << playerInd << " raised " << move.value().bet << std::endl;
                    break;
                case Actions::BET:
                    turns << "Player " << playerInd << " bet " << move.value().bet << std::endl;
                    break;
                default:
                    throw std::invalid_argument("Invalid action");
            }
            std::cout << turns.str();
        }

        // get the community cards for the round
        turns << "Community cards: ";
        switch (betRoundState) {
            case BetRoundState::PREFLOP:
                std::cout << "Flop" << std::endl;
                betRoundState = BetRoundState::FLOP;
                communityCards[0] = getCard("Enter the first community card: ");
                communityCards[1] = getCard("Enter the second community card: ");
                communityCards[2] = getCard("Enter the third community card: ");
                turns << communityCards[0].toString() << " " << communityCards[1].toString() << " " << communityCards[2].toString() << " " << std::endl;
                break;
            case BetRoundState::FLOP:
                std::cout << "Turn" << std::endl;
                betRoundState = BetRoundState::TURN;
                communityCards[3] = getCard("Enter the fourth community card: ");
                turns << communityCards[0].toString() << " " << communityCards[1].toString() << " " << communityCards[2].toString() << " " << communityCards[3].toString() << " " << std::endl;
                break;
            case BetRoundState::TURN:
                std::cout << "River" << std::endl;
                betRoundState = BetRoundState::RIVER;
                communityCards[4] = getCard("Enter the fifth community card: ");
                turns << communityCards[0].toString() << " " << communityCards[1].toString() << " " << communityCards[2].toString() << " " << communityCards[3].toString() << " "
                      << communityCards[4].toString() << " " << std::endl;
                break;
            case BetRoundState::RIVER:
                std::cout << "End of round" << std::endl;
                return;

            default:
                throw std::invalid_argument("Invalid bet round state");
        }
    }
}

int main() {
    // get game parameters
    u_int8_t numPlayers = getUC("Enter the number of players: ");
    u_int64_t smallBlind = getUL("Enter the small blind amount: ");
    u_int64_t playerChips[numPlayers];
    // get the amount of chips for each player
    getPlayersChips(numPlayers, playerChips);

    // get moves for each player
    getMoves(numPlayers, smallBlind * 2);

    // game should only last one round
    Config config{1, numPlayers, playerChips, smallBlind, 0, false, 1};
    GameTest game(config);

    // generate players
    for (int i = 0; i < numPlayers; i++) {
        game.getPlayers()[i] = std::make_unique<TestPlayer>(i);
    }

    return 0;
}