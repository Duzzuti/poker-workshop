#pragma once

#include "testc_utils.h"

/// @brief Generates a string representing a Card object in c++
/// @param card The card to generate the string for
/// @return The string representing the card
/// @exception Guarantee No-throw
/// @note The string will be in the format "Card{rank, suit}"
/// @note If the rank is 0, the string will be "Deck::getRandomCardExceptAdd(drawnCards)" to generate a random card
std::string streamCard(Card card) noexcept {
    if (card.rank == 0) return "Deck::getRandomCardExceptAdd(drawnCards)";
    return "Card{" + std::to_string(card.rank) + ", " + std::to_string(card.suit) + "}";
}

/// @brief Compiles the test configuration for the game (TestConfig that holds all the data for a test)
/// @param outputFile The file to write to (should be gametests/*_gametest.cpp)
/// @param testConfig The test configuration that should be compiled to the file
/// @exception Guarantee No-throw
/// @note Should be called in the beginning of every test
void compileTestConfig(std::ofstream& outputFile, const TestConfig& testConfig) noexcept {
    // game configuration data
    outputFile << "\tstd::vector<Card> drawnCards = {";
    bool first = true;
    for (u_int8_t i = 0; i < MAX_DRAWN_CARDS; i++) {
        if (testConfig.drawnCards[i].rank == 0) continue;
        if (!first) outputFile << ", ";
        first = false;
        outputFile << streamCard(testConfig.drawnCards[i]);
    }
    outputFile << "};" << std::endl;
    outputFile << "\tTestConfig testConfig{" << std::endl << "\t";
    outputFile << "\t.numPlayers = " << +testConfig.numPlayers << "," << std::endl << "\t";
    outputFile << "\t.smallBlind = " << testConfig.smallBlind << "," << std::endl << "\t";
    outputFile << "\t.playerChips = {";
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        outputFile << testConfig.playerChips[i];
        if (i != testConfig.numPlayers - 1) outputFile << ", ";
    }
    outputFile << "}," << std::endl << "\t";
    outputFile << "\t.playerHands = {";
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        outputFile << "{" << streamCard(testConfig.playerHands[i].first) << ", ";
        outputFile << streamCard(testConfig.playerHands[i].second) << "}";
        if (i != testConfig.numPlayers - 1) outputFile << ",";
        outputFile << std::endl << "\t\t";
        if (i != testConfig.numPlayers - 1) outputFile << "\t\t\t\t";
    }
    outputFile << "}," << std::endl << "\t";
    outputFile << "\t.drawnCards = {}," << std::endl << "\t";
    outputFile << "\t.communityCards = {";
    for (u_int8_t i = 0; i < 5; i++) {
        outputFile << streamCard(testConfig.communityCards[i]);
        if (i != 4) outputFile << ", ";
    }
    outputFile << "}," << std::endl << "\t";
    outputFile << "\t.playerActions = {" << std::endl << "\t";
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        outputFile << "\t\t{";
        for (u_int8_t j = 0; j < testConfig.playerActions[i].size(); j++) {
            outputFile << "Action{Actions::" << EnumToString::enumToString(testConfig.playerActions[i][j].action);
            outputFile << ", " << testConfig.playerActions[i][j].bet << "}";
            if (j != testConfig.playerActions[i].size() - 1) outputFile << ", ";
        }
        outputFile << "}";
        if (i != testConfig.numPlayers - 1) outputFile << ",";
        outputFile << std::endl << "\t";
    }
    outputFile << "\t}," << std::endl << "\t";
    // expected result data
    outputFile << "\t.resultData{" << std::endl << "\t";
    outputFile << "\t\t.outPlayers = {" << std::endl << "\t\t\t\t";
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        outputFile << (testConfig.resultData.outPlayers[i] ? "true" : "false");
        if (i != testConfig.numPlayers - 1) outputFile << ", ";
    }
    outputFile << std::endl << "\t\t\t}," << std::endl << "\t";
    outputFile << "\t\t.foldedPlayers = {" << std::endl << "\t\t\t\t";
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        outputFile << (testConfig.resultData.foldedPlayers[i] ? "true" : "false");
        if (i != testConfig.numPlayers - 1) outputFile << ", ";
    }
    outputFile << std::endl << "\t\t\t}," << std::endl << "\t";
    outputFile << "\t\t.nonOutPlayers = " << +testConfig.resultData.nonOutPlayers << "," << std::endl << "\t";
    outputFile << "\t\t.numActivePlayers = " << +testConfig.resultData.numActivePlayers << "," << std::endl << "\t";
    outputFile << "\t\t.gameWins = {" << std::endl << "\t\t\t\t";
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        outputFile << +testConfig.resultData.gameWins[i];
        if (i != testConfig.numPlayers - 1) outputFile << ", ";
    }
    outputFile << std::endl << "\t\t\t}," << std::endl << "\t";
    outputFile << "\t\t.pot = " << testConfig.resultData.pot << "," << std::endl << "\t";
    outputFile << "\t\t.playerChips = {" << std::endl << "\t\t\t\t";
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        outputFile << +testConfig.resultData.playerChips[i];
        if (i != testConfig.numPlayers - 1) outputFile << ", ";
    }
    outputFile << std::endl << "\t\t\t}," << std::endl << "\t";
    outputFile << "\t\t.betRoundState = BetRoundState::" << EnumToString::enumToString(testConfig.resultData.betRoundState) << "," << std::endl << "\t";
    outputFile << "\t}" << std::endl << "\t";
    outputFile << "};" << std::endl;
}

/// @brief Compiles the checks for the result data of the game
/// @param outputFile The file to write to (should be gametests/*_gametest.cpp)
/// @exception Guarantee No-throw
/// @note Should be called after the game has run in every test
void compileResultDataCheck(std::ofstream& outputFile) noexcept {
    outputFile << "\t// check if the game has run as expected" << std::endl;
    outputFile << "\tData data = game.getData();" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.numPlayers, data.numPlayers);" << std::endl;
    outputFile << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.resultData.nonOutPlayers, data.gameData.numNonOutPlayers);" << std::endl;
    outputFile << "\tfor (int i = 0; i < testConfig.numPlayers; i++) {" << std::endl;
    outputFile << "\t\tEXPECT_EQ(testConfig.resultData.outPlayers[i], data.gameData.playerOut[i]);" << std::endl;
    outputFile << "\t}" << std::endl;
    outputFile << "\tfor (int i = 0; i < testConfig.numPlayers; i++) {" << std::endl;
    outputFile << "\t\tEXPECT_EQ(testConfig.resultData.playerChips[i], data.gameData.playerChips[i]);" << std::endl;
    outputFile << "\t}" << std::endl;
    outputFile << "\tfor (int i = 0; i < testConfig.numPlayers; i++) {" << std::endl;
    outputFile << "\t\tEXPECT_EQ(testConfig.resultData.gameWins[i], data.gameData.gameWins[i]);" << std::endl;
    outputFile << "\t}" << std::endl;
    outputFile << std::endl;
    outputFile << "\tEXPECT_EQ(0, data.roundData.addBlind);" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.resultData.betRoundState, data.roundData.betRoundState);" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.smallBlind * 2, data.roundData.bigBlind);" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.smallBlind, data.roundData.smallBlind);" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.numPlayers == 2 ? 1 : 2, data.roundData.bigBlindPos);" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.numPlayers == 2 ? 0 : 1, data.roundData.smallBlindPos);" << std::endl;
    outputFile << "\tEXPECT_EQ(0, data.roundData.dealerPos);" << std::endl;
    outputFile << "\tu_int8_t communityCardsCount = data.roundData.betRoundState == BetRoundState::PREFLOP ? 0" << std::endl;
    outputFile << "\t\t\t\t\t\t\t\t\t: data.roundData.betRoundState == BetRoundState::FLOP  ? 3" << std::endl;
    outputFile << "\t\t\t\t\t\t\t\t\t: data.roundData.betRoundState == BetRoundState::TURN  ? 4" << std::endl;
    outputFile << "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t: 5;" << std::endl;
    outputFile << "\tfor (int i = 0; i < communityCardsCount; i++) {" << std::endl;
    outputFile << "\t\tEXPECT_EQ(testConfig.communityCards[i], data.roundData.communityCards[i]);" << std::endl;
    outputFile << "\t}" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.resultData.numActivePlayers, data.roundData.numActivePlayers);" << std::endl;
    outputFile << "\tfor (int i = 0; i < testConfig.numPlayers; i++) {" << std::endl;
    outputFile << "\t\tEXPECT_EQ(testConfig.resultData.foldedPlayers[i], data.roundData.playerFolded[i]);" << std::endl;
    outputFile << "\t}" << std::endl;
    outputFile << "\tEXPECT_EQ(testConfig.resultData.pot, data.roundData.pot);" << std::endl;
    outputFile << "\tEXPECT_EQ(OutEnum::GAME_WON, data.roundData.result);" << std::endl;
}

/// @brief Compiles one test file with all tests which should be included in that file
/// @param outputFile The file to write to (should be gametests/*_gametest.cpp)
/// @param fileConfig The file configuration to correctly set up the test file
/// @exception Guarantee No-throw
/// @note Writes every test from FileConfig.config to the file
void compileFile(std::ofstream& outputFile, const FileConfig& fileConfig) noexcept {
    // write the header of the file
    outputFile << "#include <gtest/gtest.h>" << std::endl;
    outputFile << "" << std::endl;
    outputFile << "#include \"game_test.h\"" << std::endl;
    outputFile << "#include \"test_player/test_player.h\"" << std::endl;
    outputFile << "" << std::endl;
    // write the tests
    for (const TestConfig& testConfig : fileConfig.config) {
        outputFile << "TEST(" << testConfig.className << ", " << testConfig.testName << ") {" << std::endl;
        // init the test config
        outputFile << "\tfor(int iters = 0; iters < TEST_ITERS; iters++){" << std::endl;
        compileTestConfig(outputFile, testConfig);
        outputFile << "\t\t// game should only last one round and not shuffle players or deck" << std::endl;
        outputFile << "\t\tConfig config{1, testConfig.numPlayers, testConfig.playerChips, testConfig.smallBlind, 0, false, false, 1};" << std::endl;
        outputFile << "\t\tGameTest game(config);" << std::endl;
        outputFile << "\t\t// build the deck for the game" << std::endl;
        outputFile << "\t\tgame.buildDeck(testConfig.playerHands, testConfig.numPlayers, testConfig.communityCards);" << std::endl;
        outputFile << std::endl;
        outputFile << "\t\t// generate players and their actions for the game" << std::endl;
        outputFile << "\t\tfor (int i = 0; i < testConfig.numPlayers; i++) {" << std::endl;
        outputFile << "\t\t\tstd::unique_ptr<TestPlayer> testPlayer = std::make_unique<TestPlayer>(i);" << std::endl;
        outputFile << "\t\t\tif (testConfig.playerActions[i].size() > 0) testPlayer->setActions(&testConfig.playerActions[i][0], testConfig.playerActions[i].size());" << std::endl;
        outputFile << "\t\t\tgame.getPlayers()[i] = std::move(testPlayer);" << std::endl;
        outputFile << "\t\t}" << std::endl;
        outputFile << std::endl;
        outputFile << "\t\t// run the game without setting new players" << std::endl;
        outputFile << "\t\tgame.run(false);" << std::endl;
        outputFile << std::endl;
        // add the checks for the result data
        compileResultDataCheck(outputFile);
        outputFile << "\t}" << std::endl;
        outputFile << "}" << std::endl << std::endl;
    }
}

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
        cmakeLists << "add_executable(gtestc_" << fileConfig.cmakeTestName << " main_test.cpp " << fileConfig.fileName
                   << " ${SRC_DIR}/game.cpp ${COMMON_SRC} ${TEST_PLAYER} ${RAND_PLAYER} ${CHECK_PLAYER})" << std::endl;
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
