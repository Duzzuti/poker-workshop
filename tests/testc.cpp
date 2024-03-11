#include <filesystem>
#include <fstream>
#include <optional>

#include "game_test.h"
#include "test_player/test_player.h"
#include "testc/testc_compiler.h"
#include "testc/testc_manual.h"
#include "testc/testc_parser.h"

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

/// @brief Checks if the file configurations are valid except for empty (random) cards
/// @param fileConfigs The file configurations to check
/// @return True if the file configurations are valid, false otherwise
/// @exception Guarantee Strong
/// @throw std::logic_error If something is invalid that should be handled by the parser
/// @note 1. Check for empty or duplicate file or cmake test names
/// @note 2. Check for empty test configurations
/// @note 3. Check for empty or duplicate class and test name combinations
/// @note 4. Check for invalid number of players, small blind and player chips
/// @note 5. Check for empty player actions
/// @note 6. Check for invalid community and player hand cards (ignoring empty (random) cards)
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
                    std::cerr << "No moves found for player " << +i << errorLocation << std::endl;
                    return false;
                }
            }
            // check for invalid community cards
            std::vector<Card> drawnCards{};
            for (u_int8_t i = 0; i < 5; i++) {
                drawnCards.push_back(testConfig.communityCards[i]);
                if (testConfig.communityCards[i] != Card{0, 0} && (testConfig.communityCards[i].rank < 2 || testConfig.communityCards[i].rank > 14 || testConfig.communityCards[i].suit > 3)) {
                    throw std::logic_error("Invalid community card (" + std::to_string(testConfig.communityCards[i].suit) + " " + std::to_string(testConfig.communityCards[i].rank) + ")" +
                                           errorLocation);
                }
            }
            // check for invalid player hand cards
            for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
                drawnCards.push_back(testConfig.playerHands[i].first);
                drawnCards.push_back(testConfig.playerHands[i].second);
                if (testConfig.playerHands[i].first != Card{0, 0} &&
                    (testConfig.playerHands[i].first.rank < 2 || testConfig.playerHands[i].first.rank > 14 || testConfig.playerHands[i].first.suit > 3)) {
                    throw std::logic_error("Invalid player hand card (" + std::to_string(testConfig.playerHands[i].first.suit) + " " + std::to_string(testConfig.playerHands[i].first.rank) + ")" +
                                           errorLocation);
                }
                if (testConfig.playerHands[i].second != Card{0, 0} &&
                    (testConfig.playerHands[i].second.rank < 2 || testConfig.playerHands[i].second.rank > 14 || testConfig.playerHands[i].second.suit > 3)) {
                    throw std::logic_error("Invalid player hand card (" + std::to_string(testConfig.playerHands[i].second.suit) + " " + std::to_string(testConfig.playerHands[i].second.rank) + ")" +
                                           errorLocation);
                }
            }
            // check for duplicate cards
            for (u_int8_t i = 0; i < drawnCards.size(); i++) {
                for (u_int8_t j = i + 1; j < drawnCards.size(); j++) {
                    if (drawnCards[i] == drawnCards[j] && drawnCards[i] != Card{0, 0}) throw std::logic_error("Duplicate card (" + std::string(drawnCards[i].toString()) + ")" + errorLocation);
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

        // format the files
        if (system("bash format.sh") != 0) {
            std::cerr << "The files could not be formatted." << std::endl;
            return 1;
        }
    }

    // *********************************************** SIMULATOR ***********************************************
    // if argc == 3, simulate the test with the given index
    if (argc == 3) {
        // get the test index
        int64_t testIndex = std::stol(argv[2]);
        if (testIndex < 0 || testIndex >= (int64_t)fileConfigs[0].config.size()) {
            std::cerr << "Invalid test index (" << testIndex << ") only " << fileConfigs[0].config.size() << " tests available for this file" << std::endl;
            return 1;
        }
        simulateTest(fileConfigs[0].config[testIndex]);
    }

    // *********************************************** BUILD ***************************************************
    // build the new generated files
    if (argc != 3 && !fileConfigs.empty()) {
        if (system("cmake -B build") != 0) {
            std::cerr << "The project could not be configured." << std::endl;
            return 1;
        }
        std::string buildCommand = "cmake --build build --target ";
        for (const FileConfig& fileConfig : fileConfigs) {
            buildCommand += "gtestc_" + fileConfig.cmakeTestName + " ";
        }
        buildCommand += "-j 20";
        if (system(buildCommand.c_str()) != 0) {
            std::cerr << "The project could not be built." << std::endl;
            return 1;
        }
    }

    return 0;
}
