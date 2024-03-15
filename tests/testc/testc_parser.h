#pragma once

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
        } else if (line.rfind("END_MOVES") == 0) {
            // end of moves found
            return;
        } else if (!isLineEmpty(line)) {
            // invalid line found
            std::cerr << "Invalid format. Expect move, END_MOVES token or blank line (line: " << currentLine << ")" << std::endl;
            exit(1);
        }
    } while (std::getline(file, line));
}

/// @brief Parses the expected result data for the test and stores it in the testConfig
/// @param file The file to read from (the current (already read) line should be the END_MOVES token)
/// @param currentLine The current line index in the file
/// @param testConfig The configuration for the test, where the result data will be stored
/// @return The next test header (line where the next test is defined) or an empty string if no more tests are found
/// @exception Guarantee No-throw
/// @note The file, currentLine and testConfig will be modified in-place
/// @note If any token is not found or ill-formed, the program will exit
/// @see To parse a whole test: parseTest()
std::string parseResultData(std::ifstream& file, u_int64_t& currentLine, TestConfig& testConfig) noexcept {
    std::string line;
    std::string playerOutLine;
    std::string playerFoldLine;
    std::string winnerLine;
    std::string gameStageLine;
    std::string potLine;
    std::string playerChipsLine;
    while (std::getline(file, line)) {
        currentLine++;
        removeComments(line);
        if (playerOutLine.empty() && line.rfind("PLAYER_OUT:", 0) == 0) {
            playerOutLine = findCharacterString(line, currentLine, 11);
            if (playerOutLine.size() != testConfig.numPlayers) {
                std::cerr << "Invalid number of chars in PLAYER_OUT token, needs to match with PLAYER_NUM (line: " << currentLine << ")" << std::endl;
                exit(1);
            }
            for (u_int8_t i = 0; i < playerOutLine.size(); i++) {
                if (playerOutLine[i] == 'f') {
                    testConfig.resultData.outPlayers[i] = false;
                    testConfig.resultData.nonOutPlayers++;
                } else if (playerOutLine[i] == 't')
                    testConfig.resultData.outPlayers[i] = true;
                else {
                    std::cerr << "Invalid character in PLAYER_OUT token, only 'f' and 't' are allowed (line: " << currentLine << ", char: " << playerOutLine[i] << ")" << std::endl;
                    exit(1);
                }
            }
        } else if (winnerLine.empty() && line.rfind("WINNER:", 0) == 0) {
            winnerLine = findCharacterString(line, currentLine, 7);
            if (winnerLine.size() != testConfig.numPlayers) {
                std::cerr << "Invalid number of chars in WINNER token, needs to match with PLAYER_NUM (line: " << currentLine << ")" << std::endl;
                exit(1);
            }
            for (u_int8_t i = 0; i < winnerLine.size(); i++) {
                if (winnerLine[i] == 'f')
                    testConfig.resultData.gameWins[i] = 0;
                else if (winnerLine[i] == 't')
                    testConfig.resultData.gameWins[i] = 1;
                else {
                    std::cerr << "Invalid character in WINNER token, only 'f' and 't' are allowed (line: " << currentLine << ", char: " << winnerLine[i] << ")" << std::endl;
                    exit(1);
                }
            }
        } else if (gameStageLine.empty() && line.rfind("GAME_STAGE:", 0) == 0) {
            gameStageLine = findCharacterString(line, currentLine, 11);
            if (gameStageLine.size() != 1) {
                std::cerr << "Invalid number of chars in GAME_STAGE token, needs to be 1 (line: " << currentLine << ")" << std::endl;
                exit(1);
            }
            if (gameStageLine[0] == 'p')
                testConfig.resultData.betRoundState = BetRoundState::PREFLOP;
            else if (gameStageLine[0] == 'f')
                testConfig.resultData.betRoundState = BetRoundState::FLOP;
            else if (gameStageLine[0] == 't')
                testConfig.resultData.betRoundState = BetRoundState::TURN;
            else if (gameStageLine[0] == 'r')
                testConfig.resultData.betRoundState = BetRoundState::RIVER;
            else {
                std::cerr << "Invalid character in GAME_STAGE token, only 'p', 'f', 't' and 'r' are allowed (line: " << currentLine << ", char: " << gameStageLine[0] << ")" << std::endl;
                exit(1);
            }
        } else if (playerFoldLine.empty() && line.rfind("PLAYER_FOLD:", 0) == 0) {
            playerFoldLine = findCharacterString(line, currentLine, 12);
            if (playerFoldLine.size() != testConfig.numPlayers) {
                std::cerr << "Invalid number of chars in PLAYER_FOLD token, needs to match with PLAYER_NUM (line: " << currentLine << ")" << std::endl;
                exit(1);
            }
            for (u_int8_t i = 0; i < playerFoldLine.size(); i++) {
                if (playerFoldLine[i] == 'f')
                    testConfig.resultData.foldedPlayers[i] = false;
                else if (playerFoldLine[i] == 't')
                    testConfig.resultData.foldedPlayers[i] = true;
                else {
                    std::cerr << "Invalid character in PLAYER_FOLD token, only 'f' and 't' are allowed (line: " << currentLine << ", char: " << playerFoldLine[i] << ")" << std::endl;
                    exit(1);
                }
            }
        } else if (playerChipsLine.empty() && line.rfind("PLAYER_CHIPS:", 0) == 0) {
            playerChipsLine = line;  // mark it as found
            u_int16_t index = 13;
            for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
                findNumber(line, testConfig.resultData.playerChips[i], currentLine, index);
            }
        } else if (potLine.empty() && line.rfind("POT:", 0) == 0) {
            potLine = line;  // mark it as found
            testConfig.resultData.pot = findNumber(line, currentLine, 4);
        } else if (line.rfind("TEST:", 0) == 0) {
            break;
        } else if (!isLineEmpty(line)) {
            std::cerr << "Invalid format. Expect TEST token or blank line (line: " << currentLine << ")" << std::endl;
            exit(1);
        }
    }
    // check if the required lines were found
    std::string missedToken;
    if (playerOutLine == "")
        missedToken = "PLAYER_OUT";
    else if (winnerLine == "")
        missedToken = "WINNER";
    else if (gameStageLine == "")
        missedToken = "GAME_STAGE";
    else if (playerFoldLine == "")
        missedToken = "PLAYER_FOLD";
    else if (potLine == "")
        missedToken = "POT";
    else if (playerChipsLine == "")
        missedToken = "PLAYER_CHIPS";

    if (missedToken != "") {
        std::cerr << missedToken << " not found" << std::endl;
        exit(1);
    }

    // calculate numActivePlayers
    for (u_int8_t i = 0; i < testConfig.numPlayers; i++) {
        if (!testConfig.resultData.outPlayers[i] && !testConfig.resultData.foldedPlayers[i]) {
            testConfig.resultData.numActivePlayers++;
        }
    }
    // return the next test header (or an empty string if no more tests are found)
    return file.eof() ? "" : line;
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
        if (testConfig.numPlayers == 0 && line.rfind("PLAYER_NUM:", 0) == 0)
            testConfig.numPlayers = findNumber(line, currentLine, 11, 2, MAX_PLAYERS);
        else if (testConfig.smallBlind == 0 && line.rfind("SMALL_BLIND:", 0) == 0)
            testConfig.smallBlind = findNumber(line, currentLine, 12, 1);
        else if (linePlayerChips.empty() && line.rfind("PLAYER_CHIPS:", 0) == 0)
            linePlayerChips = line;  // store the line for later
        else if (linePlayerCards.empty() && line.rfind("PLAYER_CARDS:", 0) == 0)
            linePlayerCards = line;  // store the line for later
        else if (lineCommunityCards.empty() && line.rfind("COMMUNITY_CARDS:", 0) == 0) {
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
        std::cerr << missedToken << " not found until line " << currentLine << std::endl;
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

    // parse the moves for each player
    parseMoves(file, currentLine, line, testConfig);
    // parse the result data
    return parseResultData(file, currentLine, testConfig);
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
