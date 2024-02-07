#include <optional>

#include "game_test.h"
#include "test_player/test_player.h"

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
/// @param min The minimum accepted number
/// @param max The maximum accepted number
/// @return The unsigned char from the user
/// @exception Guarantee No-throw
u_int8_t getUC(const std::string askForInput, const u_int8_t min = 0, const u_int8_t max = 255) noexcept {
    std::cout << askForInput;

    int32_t inputNum;
    bool validInput = false;
    while (!validInput) {
        std::string userInput;
        std::getline(std::cin, userInput);
        try {
            inputNum = std::stoi(userInput);
            if (inputNum < min || inputNum > max) {
                std::cout << "Invalid input. Please enter a number between " << +min << " and " << +max << ": ";
                continue;
            }
            validInput = true;
        } catch (std::exception& e) {
            std::cout << "Invalid input. Please enter a number: ";
        }
    }
    return (u_int8_t)inputNum;
}

/// @brief Get a card from the user, letting the user choose between a random card and a specific card
/// @param askForInput The string to ask the user for input
/// @param drawnCards The cards that have already been drawn and should not be drawn again
/// @param drawnCardCount The number of cards that have already been drawn
/// @return The card from the user
/// @exception Guarantee No-throw
/// @note The input should be in the format <H/D/C/S><2-9/T/J/Q/K/A> or 'r' for a random card
Card getCard(const std::string askForInput, Card drawnCards[], const u_int8_t drawnCardCount) noexcept {
    std::cout << askForInput << " (or 'r' for a random card): ";
    while (true) {
        std::string userInput;
        std::getline(std::cin, userInput);
        if (userInput == "r") return Deck::getRandomCardExcept(drawnCards, drawnCardCount);
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
            Card returnCard;
            switch (userInput[0]) {
                case 'H':
                    returnCard = Card{.rank = rank, .suit = 1};
                    break;
                case 'D':
                    returnCard = Card{.rank = rank, .suit = 0};
                    break;
                case 'C':
                    returnCard = Card{.rank = rank, .suit = 3};
                    break;
                case 'S':
                    returnCard = Card{.rank = rank, .suit = 2};
                    break;
                default:
                    // invalid suit entered
                    throw std::invalid_argument("Invalid suit");
            }
            if (std::find(drawnCards, &drawnCards[drawnCardCount], returnCard) != &drawnCards[drawnCardCount]) {
                std::cout << "Card already drawn. Please enter a different card: ";
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

/// @brief Ask the user for the hand of each player
/// @param numPlayers The number of players in the game
/// @param playerHands The array to store the hand for each player
/// @param drawnCards An output array for the drawn cards. Should be long enough
/// @exception Guarantee No-throw
/// @note The user can choose to get random cards
/// @note Undefined behavior if drawnCards is not long enough (numPlayers*2)
void getPlayersHand(const u_int8_t numPlayers, std::pair<Card, Card> playerHands[], Card drawnCards[]) noexcept {
    // keep track of the cards that have been entered
    u_int8_t numCards = 0;
    for (u_int8_t i = 0; i < numPlayers; i++) {
        std::cout << "Enter the hand for player " << i + 1 << std::endl;
        playerHands[i].first = getCard("Enter the first card", drawnCards, numCards);
        drawnCards[numCards++] = playerHands[i].first;
        playerHands[i].second = getCard("Enter the second card", drawnCards, numCards);
        drawnCards[numCards++] = playerHands[i].second;
        std::cout << "Player " << i + 1 << " hand: " << playerHands[i].first.toString() << " " << playerHands[i].second.toString() << std::endl;
    }
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
            } catch (std::exception& e) {
                std::cout << "Invalid input! Try again" << std::endl;
                continue;
            }
        } else if (input[0] == 'b') {
            try {
                u_int64_t bet = std::stoull(input.substr(2));
                return Action{Actions::BET, bet};
            } catch (std::exception& e) {
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
/// @param communityCards The array to store the community cards
/// @param drawnCards The array for the drawn cards, which should not be drawn again. Should be long enough
/// @param drawnCardCount The length of the drawnCards array
/// @param playerActions The array to store the actions of each player
/// @exception Guarantee Strong
/// @throw std::invalid_argument If the gotten action or the bet round state is invalid
/// @note The input is not verified, because we want to test the game with that input
/// @note The input is not processed (players who fold are not removed from the game, etc.)
/// @note The user is informed about all previous actions before asking for the next action
/// @note Undefined behavior if drawnCards array is not long enough. Has to have at least 5 empty slots.
void getMoves(const u_int8_t numPlayers, const u_int64_t bigBlind, Card communityCards[], Card drawnCards[], u_int8_t drawnCardCount, std::vector<Action> playerActions[]) {
    BetRoundState betRoundState = BetRoundState::PREFLOP;
    bool first = true;
    // store the information about the moves and community cards
    std::stringstream turns;
    while (true) {
        // consider heads up rule (if there are only 2 players, 0 = small blind, 1 = big blind else 0 = dealer, 1 = small blind, 2 = big blind)
        int16_t playerInd = first ? ((numPlayers == 2) ? 1 : 2) : 2;
        while (true) {
            playerInd++;
            if (playerInd >= numPlayers) {
                playerInd %= numPlayers;
                turns << std::endl;
            }
            // get the move of the player
            std::cout << turns.str();
            std::optional<Action> move = getMove(playerInd, first, bigBlind);
            first = false;
            // bet round is over
            if (!move.has_value()) {
                turns << "Bet round is over" << std::endl;
                break;
            }
            playerActions[playerInd].push_back(move.value());
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
        }

        // get the community cards for the round
        turns << "Community cards: ";
        switch (betRoundState) {
            case BetRoundState::PREFLOP:
                std::cout << "Flop" << std::endl;
                betRoundState = BetRoundState::FLOP;
                communityCards[0] = getCard("Enter the first community card", drawnCards, drawnCardCount);
                drawnCards[drawnCardCount++] = communityCards[0];
                communityCards[1] = getCard("Enter the second community card", drawnCards, drawnCardCount);
                drawnCards[drawnCardCount++] = communityCards[1];
                communityCards[2] = getCard("Enter the third community card", drawnCards, drawnCardCount);
                drawnCards[drawnCardCount++] = communityCards[2];
                turns << communityCards[0].toString() << " " << communityCards[1].toString() << " " << communityCards[2].toString() << " " << std::endl;
                break;
            case BetRoundState::FLOP:
                std::cout << "Turn" << std::endl;
                betRoundState = BetRoundState::TURN;
                communityCards[3] = getCard("Enter the fourth community card", drawnCards, drawnCardCount);
                drawnCards[drawnCardCount++] = communityCards[3];
                turns << communityCards[0].toString() << " " << communityCards[1].toString() << " " << communityCards[2].toString() << " " << communityCards[3].toString() << " " << std::endl;
                break;
            case BetRoundState::TURN:
                std::cout << "River" << std::endl;
                betRoundState = BetRoundState::RIVER;
                communityCards[4] = getCard("Enter the fifth community card", drawnCards, drawnCardCount);
                drawnCards[drawnCardCount++] = communityCards[4];
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
    srand(time(NULL));  // init random seed
    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("data/log_test_gen.txt", 1024 * 1024 * 10, 3);
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);

    // get game parameters
    u_int8_t numPlayers = getUC("Enter the number of players: ", 2, MAX_PLAYERS);
    std::cout << std::endl;
    u_int64_t smallBlind = getUL("Enter the small blind amount: ");
    std::cout << std::endl;
    u_int64_t playerChips[numPlayers];
    // get the amount of chips for each player
    getPlayersChips(numPlayers, playerChips);
    std::cout << std::endl;

    std::pair<Card, Card> playerHands[numPlayers];
    Card drawnCards[numPlayers * 2 + 5];  // 2 hand cards per player and 5 community cards
    // get the hand for each player
    getPlayersHand(numPlayers, playerHands, drawnCards);
    std::cout << std::endl;

    // store the community cards
    std::vector<Action> playerActions[numPlayers];
    Card communityCards[5];

    // get moves for each player
    getMoves(numPlayers, smallBlind * 2, communityCards, drawnCards, numPlayers * 2, playerActions);
    std::cout << std::endl;

    bool shouldSimulate = true;
    bool shouldPrintData = true;

    if (shouldSimulate) {
        // game should only last one round and not shuffle players or deck
        Config config{1, numPlayers, playerChips, smallBlind, 0, false, false, 1};
        GameTest game(config);
        // build the deck for the game
        game.buildDeck(playerHands, numPlayers, communityCards);

        // generate players and their actions for the game
        for (int i = 0; i < numPlayers; i++) {
            std::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);
            if (playerActions[i].size() > 0) testPlayer->setActions(&playerActions[i][0], playerActions[i].size());
            game.getPlayers()[i] = std::move(testPlayer);
        }

        // run the game without setting new players
        game.run(false);
        if (shouldPrintData) {
            std::cout << "Game data: " << std::endl;
            // FIXME: prints invalid winner data (no winners)
            game.getData().print();
        }
    }

    return 0;
}