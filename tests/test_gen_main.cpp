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
            if (rank < 2 || rank > 14)
                throw std::invalid_argument("Invalid rank");
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
void getPlayersChips(u_int8_t numPlayers, u_int64_t playerChips[]) {
    for (u_int8_t i = 0; i < numPlayers; i++) playerChips[i] = getUL("Enter the amount of chips for player " + std::to_string(i + 1) + ": ");
}

int main() {
    u_int8_t numPlayers = getUC("Enter the number of players: ");
    u_int64_t smallBlind = getUL("Enter the small blind amount: ");
    u_int64_t playerChips[numPlayers];
    getPlayersChips(numPlayers, playerChips);

    // game should only last one round
    Config config{1, numPlayers, playerChips, smallBlind, 0, false, 1};

    return 0;
}