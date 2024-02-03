#include <iostream>

#include "config.h"

/// @brief Get an unsigned long from the user
/// @param askForInput The string to ask the user for input
/// @return The unsigned long from the user
/// @exception Guarantee No-throw
u_int64_t getUL(const std::string askForInput) {
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
u_int8_t getUC(const std::string askForInput) {
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