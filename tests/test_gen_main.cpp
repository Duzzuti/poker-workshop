#include <fstream>
#include <optional>

#include "game_test.h"
#include "game_test_manual.h"
#include "game_test_utils.h"
#include "test_player/test_player.h"

/// @brief Gets the test configuration parameters from the user
/// @param testConfig The configuration for the test, where the user input will be stored
/// @exception Guarantee Strong
/// @throw std::invalid_argument If the gotten action or the bet round state is invalid
/// @note does not set fileName, testName and className
void getArgsFromUser(TestConfig& testConfig) {
    testConfig.numPlayers = getUC("Enter the number of players: ", 2, MAX_PLAYERS);
    std::cout << std::endl;
    testConfig.smallBlind = getUL("Enter the small blind amount: ");
    std::cout << std::endl;
    // get the amount of chips for each player
    getPlayersChips(testConfig.numPlayers, testConfig.playerChips);
    std::cout << std::endl;

    // get the hand for each player
    getPlayersHand(testConfig.numPlayers, testConfig.playerHands, testConfig.drawnCards);
    std::cout << std::endl;

    // get moves for each player
    getMoves(testConfig.numPlayers, testConfig.smallBlind * 2, testConfig.communityCards, testConfig.drawnCards, testConfig.numPlayers * 2, testConfig.playerActions);
    std::cout << std::endl;
}

/// @brief Generates random cards for the players and the community cards that are invalid (Card{0,0})
/// @param testConfig The configuration with the players and community cards data
/// @param drawnCardCount The amount of cards that are already drawn (length of the drawnCards array)
/// @exception Guarantee No-throw
/// @note The drawnCardCount and testConfig will be changed in place
void generateRandomCards(TestConfig& testConfig, u_int8_t& drawnCardCount) noexcept {
    // player hands
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        // first card
        if (testConfig.playerHands[i].first.rank == 0) {
            testConfig.playerHands[i].first = Deck::getRandomCardExcept(testConfig.drawnCards, drawnCardCount);
            testConfig.drawnCards[drawnCardCount++] = testConfig.playerHands[i].first;
        }
        // second card
        if (testConfig.playerHands[i].second.rank == 0) {
            testConfig.playerHands[i].second = Deck::getRandomCardExcept(testConfig.drawnCards, drawnCardCount);
            testConfig.drawnCards[drawnCardCount++] = testConfig.playerHands[i].second;
        }
    }
    // community cards
    for (u_int8_t i = 0; i < 5; i++) {
        if (testConfig.communityCards[i].rank == 0) {
            testConfig.communityCards[i] = Deck::getRandomCardExcept(testConfig.drawnCards, drawnCardCount);
            testConfig.drawnCards[drawnCardCount++] = testConfig.communityCards[i];
        }
    }
}

/// @brief Finds the n-th test in the file and the file name
/// @param file The file to search in
/// @param currentLine The current line in the file
/// @param testConfig The configuration for the test, where the file name, the test name and class will be stored
/// @param testIndex The index of the test to find
/// @exception Guarantee No-throw
/// @note The file will be read from the current position
/// @note file, currentLine and testConfig will be overriden in-place
/// @note If the test is not found, the program will exit
/// @note If the file name is not found, the program will exit
/// @see To parse a test: parseTest()
void findTest(std::ifstream& file, u_int64_t& currentLine, TestConfig& testConfig, const u_int32_t testIndex) noexcept {
    // reset the test config
    testConfig.fileName = "";
    testConfig.testName = "";
    testConfig.className = "";
    int32_t currentTest = -1;
    std::string line;
    while (std::getline(file, line)) {
        currentLine++;
        removeComments(line);
        // check if the line starts with the tokens
        if (line.rfind("FILE_NAME:", 0) == 0) testConfig.fileName = findCharacterString(line, currentLine, 10, true);
        if (line.rfind("TEST:", 0) == 0) {
            // one test found
            if (testConfig.fileName == "") {
                // file name not found yet
                std::cerr << "FILE_NAME not found" << std::endl;
                exit(1);
            }
            currentTest++;
        }
        // check if the current test is the one we are looking for
        if ((u_int32_t)currentTest == testIndex) break;
    }
    // check if the test was found
    if (currentTest == -1 || (u_int32_t)currentTest != testIndex) {
        std::cerr << "TEST " << testIndex << " not found" << std::endl;
        exit(1);
    }
    // search test name and class
    u_int16_t testNameIndex = 5;
    findCharacterString(line, testConfig.className, currentLine, testNameIndex);
    findCharacterString(line, testConfig.testName, currentLine, testNameIndex);
    PLOG_VERBOSE << "Test name: " << testConfig.testName << " in class " << testConfig.className << " from file " << testConfig.fileName << std::endl;
}

/// @brief Parses the moves for each player in the test and stores them in the testConfig
/// @param file The file to read from (the current (already read) line should be the first line of the moves)
/// @param currentLine The current line in the file
/// @param firstLine The first (already read) line of the moves
/// @param testConfig The configuration for the test, where the moves will be stored
/// @exception Guarantee No-throw
/// @note The file, currentLine and testConfig will be modified in-place
/// @note If illegal characters or ill-formed moves are found, the program will exit
/// @note Undefined behavior if the testConfig is not valid
/// @see To parse a whole test: parseTest()
void parseMoves(std::ifstream& file, u_int64_t& currentLine, const std::string& firstLine, TestConfig& testConfig) noexcept {
    std::string line = firstLine;
    u_int64_t playerIndex;
    currentLine--;  // the first line was already read
    do {
        currentLine++;
        removeComments(line);
        if (line[0] == 'P') {
            // get the player index
            u_int16_t lineColumn = 1;
            findNumber(line, playerIndex, currentLine, lineColumn, 0, testConfig.numPlayers - 1, true);
            while (line[lineColumn] != ':') {
                lineColumn++;
                if (lineColumn >= line.size()) {
                    std::cerr << "Could not find ':' after the player index in line: " << currentLine << std::endl;
                    exit(1);
                }
            }
            // skip the ':'
            lineColumn++;
            if (lineColumn >= line.size()) {
                std::cerr << "No move found after the player index in line: " << currentLine << std::endl;
                exit(1);
            }
            // get the move string
            std::string moveString = findCharacterString(line, currentLine, lineColumn);
            switch (moveString[0]) {
                case 'f':
                    // fold
                    testConfig.playerActions[playerIndex].push_back(Action{Actions::FOLD});
                    break;
                case 'c':
                    // check
                    if (moveString.size() == 3 && moveString[1] == 'h' && moveString[2] == 'k') testConfig.playerActions[playerIndex].push_back(Action{Actions::CHECK});
                    // call
                    else if (moveString.size() == 1)
                        testConfig.playerActions[playerIndex].push_back(Action{Actions::CALL});
                    else {
                        std::cerr << "Invalid move (" << moveString << ") found in line: " << currentLine << std::endl;
                        exit(1);
                    }
                    break;
                case 'r':
                    // raise
                    testConfig.playerActions[playerIndex].push_back(Action{Actions::RAISE, findNumber(moveString, currentLine, 1, 1)});
                    break;
                case 'b':
                    // bet
                    testConfig.playerActions[playerIndex].push_back(Action{Actions::BET, findNumber(moveString, currentLine, 1, 1)});
                    break;
                default:
                    std::cerr << "Invalid move (" << moveString << ") found in line: " << currentLine << std::endl;
                    exit(1);
            }
        } else if (line.rfind("TEST:") == 0) {
            // next test found (end of moves)
            break;
        } else if (!isLineEmpty(line)) {
            // invalid line found
            std::cerr << "Invalid format. Expect move, TEST token or blank line (line: " << currentLine << ")" << std::endl;
            exit(1);
        }
    } while (std::getline(file, line));
}

/// @brief Parses one test from the file and stores the configuration in the testConfig
/// @param file The file to read from (the next line should be the first line of the test (after the TEST: token))
/// @param currentLine The current line in the file
/// @param testConfig The configuration for the test, where the test parameters will be stored
/// @exception Guarantee No-throw
/// @note The file, currentLine and testConfig will be modified in-place
/// @note If the test parameters are not found, the program will exit
/// @see To find a test: findTest()
void parseTest(std::ifstream& file, u_int64_t& currentLine, TestConfig& testConfig) noexcept {
    std::string line;
    std::string linePlayerChips;
    std::string linePlayerCards;
    std::string lineCommunityCards;
    // keeps track of the amount of cards that are drawn
    u_int8_t drawnCardCount = 0;
    while (std::getline(file, line)) {
        currentLine++;
        removeComments(line);
        // check if the line starts with the tokens
        if (line.rfind("PLAYER_NUM:", 0) == 0)
            testConfig.numPlayers = findNumber(line, currentLine, 11, 2, MAX_PLAYERS);
        else if (line.rfind("SMALL_BLIND:", 0) == 0)
            testConfig.smallBlind = findNumber(line, currentLine, 12, 1);
        else if (line.rfind("PLAYER_CHIPS:", 0) == 0)
            linePlayerChips = line;  // store the line for later
        else if (line.rfind("PLAYER_CARDS:", 0) == 0)
            linePlayerCards = line;  // store the line for later
        else if (line.rfind("COMMUNITY_CARDS:", 0) == 0) {
            // saves the community cards directly
            u_int16_t index = 16;
            findCharacterString(line, lineCommunityCards, currentLine, index);
            index = 0;
            for (u_int8_t i = 0; i < 5; i++) {
                testConfig.communityCards[i] = findCard(lineCommunityCards, currentLine, index, testConfig.drawnCards, drawnCardCount);
            }
        } else if (!isLineEmpty(line)) {
            // if the line is not empty and does not start with a token, the test parameters are over (or the file is invalid)
            break;
        }
    }
    // check if the required lines were found
    std::string missedToken;
    if (testConfig.numPlayers == 0)
        missedToken = "PLAYER_NUM";
    else if (testConfig.smallBlind == 0)
        missedToken = "SMALL_BLIND";
    else if (linePlayerChips == "")
        missedToken = "PLAYER_CHIPS";
    else if (linePlayerCards == "")
        missedToken = "PLAYER_CARDS";
    else if (lineCommunityCards == "")
        missedToken = "COMMUNITY_CARDS";

    if (missedToken != "") {
        std::cerr << missedToken << " not found" << std::endl;
        exit(1);
    }

    // get the amount of chips for each player
    u_int16_t index = 13;
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        findNumber(linePlayerChips, testConfig.playerChips[i], currentLine, index);
    }
    // get the hand for each player
    index = 14;
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        std::string playerPart;
        findCharacterString(linePlayerCards, playerPart, currentLine, index);
        u_int16_t handIndex = 0;
        testConfig.playerHands[i].first = findCard(playerPart, currentLine, handIndex, testConfig.drawnCards, drawnCardCount);
        testConfig.playerHands[i].second = findCard(playerPart, currentLine, handIndex, testConfig.drawnCards, drawnCardCount);
    }

    // generate the random cards that are represented by invalid placeholders (Card{0,0})
    generateRandomCards(testConfig, drawnCardCount);
    // parse the moves for each player
    parseMoves(file, currentLine, line, testConfig);
}

/// @brief Returns the configuration for the n-th test in the file
/// @param file The file to read from
/// @param testIndex The index of the test to find
/// @return The configuration for the n-th test in the file
/// @exception Guarantee No-throw
/// @note If the test is not found or invalid, the program will exit
TestConfig parseFile(std::ifstream& file, const u_int32_t testIndex) noexcept {
    // search for the n-th test
    TestConfig testConfig;
    testConfig.fileName = "";
    testConfig.testName = "";
    testConfig.className = "";
    testConfig.numPlayers = 0;
    testConfig.smallBlind = 0;
    u_int64_t currentLine = 0;

    findTest(file, currentLine, testConfig, testIndex);

    parseTest(file, currentLine, testConfig);

    return testConfig;
}

/// @brief Simulates a test with the given parameters
/// @param testConfig The configuration for the test
/// @param printData Whether the game data should be printed after the simulation
/// @exception Guarantee Strong
/// @throw std::invalid_argument If the test configuration is invalid
/// @throw std::logic_error If something went wrong during the simulation
/// @note The game should only last one round and not shuffle players or deck
void simulateTest(const TestConfig& testConfig, const bool printData = true) {
    // game should only last one round and not shuffle players or deck
    Config config{1, testConfig.numPlayers, testConfig.playerChips, testConfig.smallBlind, 0, false, false, 1};
    GameTest game(config);
    // build the deck for the game
    game.buildDeck(testConfig.playerHands, testConfig.numPlayers, testConfig.communityCards);

    // generate players and their actions for the game
    for (int i = 0; i < testConfig.numPlayers; i++) {
        std::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);
        if (testConfig.playerActions[i].size() > 0) testPlayer->setActions(&testConfig.playerActions[i][0], testConfig.playerActions[i].size());
        game.getPlayers()[i] = std::move(testPlayer);
    }

    // run the game without setting new players
    game.run(false);
    std::cout << "Game data: " << std::endl;
    // FIXME: prints invalid winner data (no winners)
    if (printData) game.getData().print();
}

// TODO: the description is not fully implemented yet
/// @brief Main function for the test generator (test compiler)
/// @note If nothing is provided, the user will be prompted for the game parameters
/// @note If a file is provided, the file will be compiled
/// @note If a file and a test index are provided, the game parameters will be read from the file and the n-th test will be simulated
int main(int argc, char* argv[]) {
    srand(time(NULL));  // init random seed
    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("data/log_test_gen.txt", 1024 * 1024 * 10, 3);
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);

    TestConfig testConfig;

    if (argc == 2 || argc == 3) {
        // get game parameters from file
        std::string fileName = argv[1];
        u_int32_t testIndex = 0;
        if (argc == 3) {
            try {
                testIndex = std::stoi(argv[2]);
            } catch (std::exception& e) {
                std::cerr << "Invalid test index provided" << std::endl;
                return 1;
            }
        }
        std::ifstream file(fileName, std::ios::in);
        if (!file.is_open()) {
            file = std::ifstream("testscripts/" + fileName, std::ios::in);
            if (!file.is_open()) {
                file = std::ifstream("tests/testscripts/" + fileName, std::ios::in);
                if (!file.is_open()) {
                    std::cerr << "File " << fileName << " not found" << std::endl;
                    return 1;
                }
            }
        }

        testConfig = parseFile(file, testIndex);

    } else if (argc == 1) {
        // get game parameters manually
        getArgsFromUser(testConfig);
    } else {
        std::cerr << "Invalid number of arguments" << std::endl;
        return 1;
    }

    simulateTest(testConfig);

    return 0;
}
