#include <filesystem>
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
    testConfig.className = "GameTest";
    testConfig.testName = "test";
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

/// @brief Finds the n-th test in the file and returns the line where the test is defined (test header), storing the file name and cmake name
/// @param file The file to search in
/// @param currentLine The current line in the file
/// @param fileConfig The configuration for the file, where the file name and cmake name will be stored
/// @param testIndex The index of the test to find
/// @param exitOnMiss Whether the program should exit if the test is not found (or return an empty string)
/// @return The line where the n-th test is defined
/// @exception Guarantee No-throw
/// @note The file will be read from the current position
/// @note file, currentLine and fileConfig will be overriden in-place
/// @note If the test is not found, the program will exit
/// @note If the file name or cmake name is not found, the program will exit
/// @see To parse the test header: parseTestHeader()
std::string findTestLine(std::ifstream& file, u_int64_t& currentLine, FileConfig& fileConfig, const u_int32_t testIndex, const bool exitOnMiss = true) noexcept {
    // reset the file config
    fileConfig.fileName = "";
    fileConfig.cmakeTestName = "";
    int32_t currentTest = -1;
    std::string line;
    while (std::getline(file, line)) {
        currentLine++;
        removeComments(line);
        // check if the line starts with the tokens
        if (line.rfind("FILE_NAME:", 0) == 0) fileConfig.fileName = findCharacterString(line, currentLine, 10, true);
        if (line.rfind("CMAKE_TEST_NAME:", 0) == 0) fileConfig.cmakeTestName = findCharacterString(line, currentLine, 16);
        if (line.rfind("TEST:", 0) == 0) {
            // one test found
            if (fileConfig.fileName == "") {
                // file name not found yet
                std::cerr << "FILE_NAME not found" << std::endl;
                exit(1);
            } else if (fileConfig.cmakeTestName == "") {
                // cmake test name not found yet
                std::cerr << "CMAKE_TEST_NAME not found" << std::endl;
                exit(1);
            }
            currentTest++;
        }
        // check if the current test is the one we are looking for
        if ((u_int32_t)currentTest == testIndex) return line;
    }
    if (exitOnMiss) {
        std::cerr << "TEST " << testIndex << " not found" << std::endl;
        exit(1);
    }
    return "";
}

/// @brief Parses the test header and stores the class name and test name in the testConfig
/// @param testHeader The line where the test is defined
/// @param currentLine The current line in the file
/// @param testConfig The configuration for the test, where the class name and test name will be stored
/// @exception Guarantee No-throw
/// @note testConfig will be modified in-place
/// @note If the class name or test name is not found, the program will exit
/// @see To get the test header: findTestLine()
void parseTestHeader(const std::string& testHeader, const u_int64_t currentLine, TestConfig& testConfig) noexcept {
    u_int16_t index = 5;
    findCharacterString(testHeader, testConfig.className, currentLine, index);
    findCharacterString(testHeader, testConfig.testName, currentLine, index);
}

/// @brief Parses the moves for each player in the test and stores them in the testConfig, gets the next test header
/// @param file The file to read from (the current (already read) line should be the first line of the moves)
/// @param currentLine The current line in the file
/// @param firstLine The first (already read) line of the moves
/// @param testConfig The configuration for the test, where the moves will be stored
/// @return The next test header (line where the next test is defined) or an empty string if no more tests are found
/// @exception Guarantee No-throw
/// @note The file, currentLine and testConfig will be modified in-place
/// @note If illegal characters or ill-formed moves are found, the program will exit
/// @note Undefined behavior if the testConfig is not valid
/// @see To parse a whole test: parseTest()
std::string parseMoves(std::ifstream& file, u_int64_t& currentLine, const std::string& firstLine, TestConfig& testConfig) noexcept {
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

    if (file.eof()) return "";
    return line;
}

/// @brief Parses one test from the file and stores the configuration in the testConfig
/// @param file The file to read from (the next line should be the first line of the test (after the TEST: token))
/// @param currentLine The current line in the file
/// @param testConfig The configuration for the test, where the test parameters will be stored
/// @return The next test header (line where the next test is defined) or an empty string if no more tests are found
/// @exception Guarantee No-throw
/// @note The file, currentLine and testConfig will be modified in-place
/// @note If the test parameters are not found, the program will exit
/// @see To find a test: findTestLine()
std::string parseTest(std::ifstream& file, u_int64_t& currentLine, TestConfig& testConfig) noexcept {
    testConfig.numPlayers = 0;
    testConfig.smallBlind = 0;
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
    return parseMoves(file, currentLine, line, testConfig);
}

/// @brief Returns the file configuration but only with the n-th test in the file as a configuration
/// @param file The file to read from
/// @param testIndex The index of the test to find
/// @return The file configuration with the n-th test configuration, does only contain one test
/// @exception Guarantee No-throw
/// @note If the test is not found or invalid, the program will exit
/// @see To parse the whole file: parseFile(std::ifstream&)
FileConfig parseFile(std::ifstream& file, const u_int32_t testIndex) noexcept {
    // search for the n-th test
    FileConfig fileConfig;
    fileConfig.config.emplace_back();
    u_int64_t currentLine = 0;

    std::string testHeader = findTestLine(file, currentLine, fileConfig, testIndex);
    parseTestHeader(testHeader, currentLine, fileConfig.config[0]);

    parseTest(file, currentLine, fileConfig.config[0]);

    return fileConfig;
}

/// @brief Returns the file configuration with all tests as a configuration
/// @param file The file to read from
/// @return The file configuration with all tests configurations found in the file
/// @exception Guarantee No-throw
/// @note If the test is not found or invalid, the program will exit
/// @see To parse only one test for a file: parseFile(std::ifstream&, const u_int32_t)
FileConfig parseFile(std::ifstream& file) noexcept {
    FileConfig fileConfig;
    u_int64_t currentLine = 0;
    u_int32_t testIndex = 0;
    // find the first test
    std::string testHeader = findTestLine(file, currentLine, fileConfig, 0, false);
    while (testHeader != "") {
        // add a new test configuration
        fileConfig.config.emplace_back();
        parseTestHeader(testHeader, currentLine, fileConfig.config[testIndex]);
        testHeader = parseTest(file, currentLine, fileConfig.config[testIndex]);
        testIndex++;
    }
    return fileConfig;
}

/// @brief Compiles all files in the given directory and returns the file configurations
/// @param directory The directory to search for .gts files
/// @return The file configurations for all .gts files in the directory
/// @exception Guarantee No-throw
/// @note If there is any invalid syntax in the .gts files, the program will exit
/// @see To parse a single file: parseFile(std::ifstream&)
std::vector<FileConfig> parseFiles(const std::filesystem::path& directory) noexcept {
    std::vector<FileConfig> fileConfigs;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        // only parse .gts files
        if (std::filesystem::exists(entry) && entry.is_regular_file() && entry.path().extension() == ".gts") {
            std::ifstream file(entry.path(), std::ios::in);
            fileConfigs.push_back(parseFile(file));
        }
    }
    return fileConfigs;
}

/// @brief Checks if the file configurations are valid
/// @param fileConfigs The file configurations to check
/// @return True if the file configurations are valid, false otherwise
/// @exception Guarantee Strong
/// @throw std::logic_error If something is invalid that should be handled by the parser
/// @note 1. Check for empty or duplicate file or cmake test names
/// @note 2. Check for empty test configurations
/// @note 3. Check for empty or duplicate class and test name combinations
/// @note 4. Check for invalid number of players, small blind and player chips
/// @note 5. Check for empty player actions
/// @note 6. Check for invalid community and player hand cards
/// @note 7. Check for duplicate cards
/// @note 8. Check for duplicate class names in different files
bool checkFileConfigs(const std::vector<FileConfig>& fileConfigs) {
    std::vector<std::string> fileNames;
    std::vector<std::string> cmakeTestNames;
    std::vector<std::string> classNames;
    // check the file configurations
    for (const FileConfig& fileConfig : fileConfigs) {
        if (std::find(fileNames.begin(), fileNames.end(), fileConfig.fileName) != fileNames.end()) {
            std::cerr << "Duplicate file name found: " << fileConfig.fileName << std::endl;
            return false;
        }
        if (std::find(cmakeTestNames.begin(), cmakeTestNames.end(), fileConfig.cmakeTestName) != cmakeTestNames.end()) {
            std::cerr << "Duplicate cmake test name found: " << fileConfig.cmakeTestName << std::endl;
            return false;
        }
        fileNames.push_back(fileConfig.fileName);
        cmakeTestNames.push_back(fileConfig.cmakeTestName);
        if (fileConfig.fileName.empty()) throw std::logic_error("File name not provided in file configuration");
        if (fileConfig.cmakeTestName.empty()) throw std::logic_error("CMake test name not provided in file configuration");
        if (fileConfig.config.empty()) {
            std::cerr << "No tests found in file " << fileConfig.fileName << std::endl;
            return false;
        }
        std::vector<std::string> fileClassNames{};
        std::vector<std::string> fileClassTestNames{};
        // check the test configurations
        for (const TestConfig& testConfig : fileConfig.config) {
            std::string errorLocation = " in file " + fileConfig.fileName + " in test " + testConfig.className + "_" + testConfig.testName;
            // check for invalid class and test names, number of players, small blind
            if (std::find(fileClassNames.begin(), fileClassNames.end(), testConfig.className) == fileClassNames.end()) {
                fileClassNames.push_back(testConfig.className);
            }
            if (std::find(fileClassTestNames.begin(), fileClassTestNames.end(), testConfig.className + testConfig.testName) != fileClassTestNames.end()) {
                std::cerr << "Duplicate class + test name found: " << testConfig.className + "_" + testConfig.testName << errorLocation << std::endl;
                return false;
            }
            fileClassTestNames.push_back(testConfig.className + testConfig.testName);
            if (testConfig.className.empty()) throw std::logic_error("Class name not provided in test configuration" + errorLocation);
            if (testConfig.testName.empty()) throw std::logic_error("Test name not provided in test configuration" + errorLocation);
            if (testConfig.numPlayers < 2 || testConfig.numPlayers > MAX_PLAYERS) throw std::logic_error("Invalid number of players (" + std::to_string(testConfig.numPlayers) + ")" + errorLocation);
            if (testConfig.smallBlind < 1) throw std::logic_error("Invalid small blind (" + std::to_string(testConfig.smallBlind) + ")" + errorLocation);
            // check for invalid player chips
            for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
                if (testConfig.playerChips[i] < 1) {
                    throw std::logic_error("Invalid player chips (" + std::to_string(testConfig.playerChips[i]) + ")" + errorLocation);
                }
            }
            // check for empty player actions
            for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
                if (testConfig.playerActions[i].empty()) {
                    std::cerr << "No moves found for player " << i << errorLocation << std::endl;
                    return false;
                }
            }
            // check for invalid community cards
            std::vector<Card> drawnCards{};
            for (u_int8_t i = 0; i < 5; i++) {
                drawnCards.push_back(testConfig.communityCards[i]);
                if (testConfig.communityCards[i].rank < 2 || testConfig.communityCards[i].rank > 14 || testConfig.communityCards[i].suit > 3) {
                    throw std::logic_error("Invalid community card (" + std::to_string(testConfig.communityCards[i].suit) + " " + std::to_string(testConfig.communityCards[i].rank) + ")" +
                                           errorLocation);
                }
            }
            // check for invalid player hand cards
            for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
                drawnCards.push_back(testConfig.playerHands[i].first);
                drawnCards.push_back(testConfig.playerHands[i].second);
                if (testConfig.playerHands[i].first.rank < 2 || testConfig.playerHands[i].first.rank > 14 || testConfig.playerHands[i].first.suit > 3) {
                    throw std::logic_error("Invalid player hand card (" + std::to_string(testConfig.playerHands[i].first.suit) + " " + std::to_string(testConfig.playerHands[i].first.rank) + ")" +
                                           errorLocation);
                }
                if (testConfig.playerHands[i].second.rank < 2 || testConfig.playerHands[i].second.rank > 14 || testConfig.playerHands[i].second.suit > 3) {
                    throw std::logic_error("Invalid player hand card (" + std::to_string(testConfig.playerHands[i].second.suit) + " " + std::to_string(testConfig.playerHands[i].second.rank) + ")" +
                                           errorLocation);
                }
            }
            // check for duplicate cards
            for (u_int8_t i = 0; i < drawnCards.size(); i++) {
                for (u_int8_t j = i + 1; j < drawnCards.size(); j++) {
                    if (drawnCards[i] == drawnCards[j]) throw std::logic_error("Duplicate card (" + std::string(drawnCards[i].toString()) + ")" + errorLocation);
                }
            }
        }
        // check for duplicate class names in different files
        for (const std::string& className : fileClassNames) {
            if (std::find(classNames.begin(), classNames.end(), className) != classNames.end()) {
                std::cerr << "Duplicate class name in different files found: " << className << std::endl;
                return false;
            }
            classNames.push_back(className);
        }
    }
    return true;
}

void compileFile(std::ofstream& outputFile, const FileConfig& fileConfig) noexcept {}

/// @brief Compiles the CMakeLists.txt file for the tests
/// @param cmakeLists The file to write to (should be gametests/CMakeLists.txt)
/// @param fileConfigs The file configurations to correctly set up the cmake file
/// @exception Guarantee No-throw
void compileCmake(std::ofstream& cmakeLists, const std::vector<FileConfig>& fileConfigs) noexcept {
    cmakeLists << "FetchContent_Declare(" << std::endl;
    cmakeLists << "    googletest" << std::endl;
    cmakeLists << "    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip" << std::endl;
    cmakeLists << ")" << std::endl;
    cmakeLists << "# For Windows: Prevent overriding the parent project's compiler/linker settings" << std::endl;
    cmakeLists << "set(gtest_force_shared_crt ON CACHE BOOL \"\" FORCE)" << std::endl;
    cmakeLists << "FetchContent_MakeAvailable(googletest)" << std::endl;
    cmakeLists << "" << std::endl;
    cmakeLists << "enable_testing()" << std::endl;
    cmakeLists << "" << std::endl;
    cmakeLists << "# tests" << std::endl;
    for (const FileConfig& fileConfig : fileConfigs) {
        cmakeLists << "add_executable(gtestc_" << fileConfig.cmakeTestName << " main_test.cpp " << fileConfig.fileName << " ${COMMON_SRC} ${TEST_PLAYER})" << std::endl;
        cmakeLists << "target_link_libraries(gtestc_" << fileConfig.cmakeTestName << " gtest_main)" << std::endl;
        cmakeLists << "target_include_directories(gtestc_" << fileConfig.cmakeTestName << " PUBLIC ${INCLUDE_DIR} ${PLAYER_DIR} ${TEST_DIR})" << std::endl;
        cmakeLists << std::endl;
    }
    for (const FileConfig& fileConfig : fileConfigs) {
        cmakeLists << "add_test(GTEST_" << stringToUpper(fileConfig.cmakeTestName) << " gtestc_" << fileConfig.cmakeTestName << ")" << std::endl;
    }
}

/// @brief Compiles the main_test.cpp file for the tests
/// @param mainTest The file to write to (should be gametests/main_test.cpp)
/// @exception Guarantee No-throw
void compileMainTest(std::ofstream& mainTest) noexcept {
    mainTest << "#include <gtest/gtest.h>" << std::endl;
    mainTest << "" << std::endl;
    mainTest << "#include \"logger.h\"" << std::endl;
    mainTest << "" << std::endl;
    mainTest << "int main(int argc, char** argv) {" << std::endl;
    mainTest << "    srand(time(NULL));  // init random seed" << std::endl;
    mainTest << "    // the main function that is running the tests" << std::endl;
    mainTest << "    // init logger" << std::endl;
    mainTest << "    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;" << std::endl;
    mainTest << "    // add file logger" << std::endl;
    mainTest << "    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(\"data/log_gametest.txt\", 1024 * 1024 * 10, 3);" << std::endl;
    mainTest << "    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);" << std::endl;
    mainTest << "    // plog::init(plog::verbose, &fileAppender);" << std::endl;
    mainTest << "    testing::InitGoogleTest(&argc, argv);" << std::endl;
    mainTest << "    return RUN_ALL_TESTS();" << std::endl;
    mainTest << "}" << std::endl;
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
    if (printData) game.getData().print();
}

/// @brief Main function for the test generator (test compiler)
/// @note Has to be called from the root directory of the project
/// @note If it is compiling, the gametest directory will be cleared
/// @note If nothing is provided, the user will be prompted for the game parameters
/// @note If a file name is provided, the file with the given name in the testscripts directory will be compiled
/// @note if a directory is provided, all ".gts" files in the directory will be compiled
/// @note If a file name and a test index are provided, the game parameters will be read from the file
/// with the given name in the testscripts directory and the n-th test will be simulated (does not compile)
int main(int argc, char* argv[]) {
    srand(time(NULL));  // init random seed
    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("data/log_test_gen.txt", 1024 * 1024 * 10, 3);
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);

    // verify directory structure
    const std::filesystem::path testscriptsDir = "tests/testscripts/";
    const std::filesystem::path gametestsDir = "tests/gametests/";
    if (!std::filesystem::exists(testscriptsDir)) {
        std::cerr << "The testscripts directory is missing. Please run the program from the root directory of the project." << std::endl;
        return 1;
    }
    if (!std::filesystem::exists(gametestsDir)) {
        std::cerr << "The gametests directory is missing." << std::endl;
        return 1;
    }

    std::vector<FileConfig> fileConfigs;

    // *********************************************** PARSER ***********************************************

    if (argc == 1) {
        // get game parameters manually
        // create a new file and test configuration, use default file config
        fileConfigs.emplace_back();
        fileConfigs[0].config.emplace_back();
        getArgsFromUser(fileConfigs[0].config[0]);
    } else if (argc == 2 || argc == 3) {
        // get game parameters from file
        std::filesystem::path path = argv[1];
        if (std::filesystem::is_regular_file(testscriptsDir / path)) {
            // get game parameters from a single file
            if (std::filesystem::path(testscriptsDir / path).extension() == ".gts") {
                // file should be located in the testscripts directory
                std::ifstream file(testscriptsDir / path, std::ios::in);
                if (!file.is_open()) {
                    std::cerr << "File not found (" << testscriptsDir / path << ")" << std::endl;
                    return 1;
                }
                // get the test parameters from the file
                fileConfigs.push_back(parseFile(file));
            } else {
                std::cerr << "Invalid file extension, only compiles \".gts\" files (" << path << ")" << std::endl;
                return 1;
            }
        } else if (argc == 3) {
            std::cerr << "Provided path is not a regular file (" << testscriptsDir / path << ")" << std::endl;
            return 1;
        } else if (std::filesystem::is_directory(path)) {
            fileConfigs = parseFiles(path);
        } else {
            std::cerr << "Provided path is not a regular file or directory (" << testscriptsDir / path << " or " << path << ")" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "Invalid number of arguments (" << argc << ")" << std::endl;
        return 1;
    }

    // check if the file configs are valid
    if (!checkFileConfigs(fileConfigs)) return 1;

    // *********************************************** COMPILER ***********************************************

    // check if we have to compile (dont compile for argc == 3, see main docs)
    if (argc != 3 && !fileConfigs.empty()) {
        // clear the gametests directory
        if (!std::filesystem::remove_all(gametestsDir)) {
            std::cerr << "Could not clear the gametests directory. Please check for missing permissions." << std::endl;
            return 1;
        }
        if (!std::filesystem::create_directory(gametestsDir)) {
            std::cerr << "The gametests directory could not be created. Please check for missing permissions." << std::endl;
            return 1;
        }

        // create the main_test.cpp file
        std::ofstream mainTest(gametestsDir / "main_test.cpp", std::ios::out);
        compileMainTest(mainTest);
        mainTest.close();

        // create the CMakeLists.txt file
        std::ofstream cmakeLists(gametestsDir / "CMakeLists.txt", std::ios::out);
        compileCmake(cmakeLists, fileConfigs);
        cmakeLists.close();

        // create the test files
        for (const FileConfig& fileConfig : fileConfigs) {
            std::ofstream outputFile(gametestsDir / fileConfig.fileName, std::ios::out);
            compileFile(outputFile, fileConfig);
        }
    }

    // *********************************************** SIMULATOR ***********************************************
    // TODO simulateTest(testConfigs[testIndex]);

    // *********************************************** BUILD ***************************************************
    // TODO rebuild the project with the new files

    return 0;
}
