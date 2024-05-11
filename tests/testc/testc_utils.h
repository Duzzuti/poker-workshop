#pragma once

#include "logger.h"

/// @brief Logs a fatal message and throws std::logic_error()
/// @param message The message to log and throw
/// @exception Guarantee Strong (throws every time)
/// @throws std::logic_error every time with message as the what() string
void logFatalAndThrow(const std::string& message) {
    PLOG_FATAL << message;
    throw std::logic_error(message);
}

/// @brief Converts a string to uppercase
/// @param str The string to convert
/// @return The uppercase string
/// @exception Guarantee No-throw
std::string stringToUpper(std::string str) noexcept {
    std::string result;
    for (char c : str) result += std::toupper(c);
    return result;
}

/// @brief Removes the comments from a line
/// @param line The line to remove the comments from
/// @exception Guarantee No-throw
/// @note The line is changed in place
/// @note Comments are everything after "//"
void removeComments(std::string& line) noexcept {
    size_t commentPos = line.find("//");
    if (commentPos != std::string::npos) line = line.substr(0, commentPos);
}

/// @brief Checks if a line only contains spaces
/// @param line The line to check
/// @return True if the line only contains spaces, false otherwise
/// @exception Guarantee No-throw
bool isLineEmpty(const std::string& line) noexcept { return line.find_first_not_of(' ') == std::string::npos; }

/// @brief Exits the program with an error message
/// @param line The index of the line in the file
/// @param column The index of the character in the line
/// @param invChar The invalid character
/// @exception Guarantee No-throw
/// @note This function will exit the program
void invalidCharFound(const u_int32_t line, const u_int16_t column, const char invChar) noexcept {
    PLOG_ERROR << "Invalid character found in line " << line << " at position " << column << " (" << invChar << ")";
    std::cerr << "Invalid character found in line " << line << " at position " << column << " (" << invChar << ")" << std::endl;
    exit(1);
}

/// @brief Finds the first string of characters in a line without spaces
/// @param line The line to search
/// @param result The string to store the result in
/// @param lineInd The index of the line in the file
/// @param index The index of the character in the line to start searching from (will be changed to the end of the found string)
/// @param acceptDot If true, the string can contain dots
/// @exception Guarantee No-throw
/// @note The result and index are changed in place
/// @note Searches for the first valid character while skipping spaces, then searches for the next space or end of line to end the string
/// @note If any invalid characters are found, the program will exit
/// @see For a non-in-place version, see findCharacterString(const std::string&, const u_int32_t, const u_int16_t, const bool)
/// @see If you search numbers: findNumber()
/// @see If you search cards: findCard()
void findCharacterString(const std::string& line, std::string& result, const u_int32_t lineInd, u_int16_t& index, const bool acceptDot = false) noexcept {
    // valid characters in the string
    const std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_";
    // valid first characters in the string
    const std::string validCharsFirst = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
    // characters that are used to separate the string from other characters
    const std::string validSepChars = " ";
    bool foundFirst = false;
    for (; index < line.size(); index++) {
        if (foundFirst) {
            // check if the string ends
            if (std::find(validSepChars.begin(), validSepChars.end(), line[index]) != validSepChars.end()) break;
            // check if the character is valid
            if ((acceptDot && line[index] == '.') || std::find(validChars.begin(), validChars.end(), line[index]) != validChars.end())
                result += line[index];
            else
                invalidCharFound(lineInd, index, line[index]);
        } else {
            // still not found
            if (std::find(validSepChars.begin(), validSepChars.end(), line[index]) != validSepChars.end()) continue;

            // check if the character is valid
            if (std::find(validCharsFirst.begin(), validCharsFirst.end(), line[index]) != validCharsFirst.end()) {
                result += line[index];
                foundFirst = true;
            } else
                invalidCharFound(lineInd, index, line[index]);
        }
    }
}

/// @brief Finds the first string of characters in a line without spaces
/// @param line The line to search
/// @param lineInd The index of the line in the file
/// @param firstIndex The index of the character in the line to start searching from
/// @param acceptDot If true, the string can contain dots
/// @return The found string
/// @exception Guarantee No-throw
/// @note Searches for the first valid character while skipping spaces, then searches for the next space or end of line to end the string
/// @note If any invalid characters are found, the program will exit
/// @see For an in-place version, see findCharacterString(const std::string&, std::string&, const u_int32_t, u_int16_t&, const bool)
/// @see If you search numbers: findNumber()
/// @see If you search cards: findCard()
std::string findCharacterString(const std::string& line, const u_int32_t lineInd, const u_int16_t firstIndex, const bool acceptDot = false) noexcept {
    std::string result;
    u_int16_t index = firstIndex;
    findCharacterString(line, result, lineInd, index, acceptDot);
    return result;
}

/// @brief Finds the first number in a line without spaces
/// @param line The line to search
/// @param result The number to store the result in
/// @param lineInd The index of the line in the file
/// @param index The index of the character in the line to start searching from
/// @param min The minimum allowed value for the number
/// @param max The maximum allowed value for the number
/// @param acceptColonEnd If true, the number can end with a colon
/// @exception Guarantee No-throw
/// @note The result and index are changed in place
/// @note Searches for the first valid character while skipping spaces, then searches for the next space or end of line to end the number
/// @note If any invalid characters are found, the program will exit
/// @note If the number is out of range, the program will exit
/// @see For a non-in-place version, see findNumber(const std::string&, const u_int32_t, u_int16_t, const u_int64_t, const u_int64_t, const bool)
/// @see If you search strings: findCharacterString()
/// @see If you search cards: findCard()
void findNumber(const std::string& line, u_int64_t& result, const u_int32_t lineInd, u_int16_t& index, const u_int64_t min = 0, const u_int64_t max = -1, const bool acceptColonEnd = false) noexcept {
    const std::string validChars = "1234567890";
    // characters that are used to separate the number from other characters
    const std::string validSepChars = " ";
    result = 0;  // reset the result
    bool foundFirst = false;
    for (; index < line.size(); index++) {
        // check for separator characters
        if (std::find(validSepChars.begin(), validSepChars.end(), line[index]) != validSepChars.end()) {
            // if the number is already read, stop
            if (foundFirst) break;
            continue;
        }
        // end the number if the character is a colon and the number is already found
        if (acceptColonEnd && line[index] == ':' && foundFirst) break;

        // check if the character is valid
        if (std::find(validChars.begin(), validChars.end(), line[index]) == validChars.end()) invalidCharFound(lineInd, index, line[index]);

        // add the number to the result by shifting the current result by one digit and adding the new digit
        result *= 10;
        result += line[index] - '0';
        foundFirst = true;
    }
    // check if the number is out of range
    if (result < min || result > max) {
        PLOG_ERROR << "Number out of range (" << min << "-" << max << ") in line " << lineInd << " at position " << index << " (" << result << ")";
        std::cerr << "Number out of range (" << min << "-" << max << ") in line " << lineInd << " at position " << index << " (" << result << ")" << std::endl;
        exit(1);
    }
}

/// @brief Finds the first number in a line without spaces
/// @param line The line to search
/// @param lineInd The index of the line in the file
/// @param index The index of the character in the line to start searching from
/// @param min The minimum allowed value for the number
/// @param max The maximum allowed value for the number
/// @param acceptColonEnd If true, the number can end with a colon
/// @return The found number
/// @exception Guarantee No-throw
/// @note Searches for the first valid character while skipping spaces, then searches for the next space or end of line to end the number
/// @note If any invalid characters are found, the program will exit
/// @note If the number is out of range, the program will exit
/// @see For an in-place version, see findNumber(const std::string&, u_int64_t&, const u_int32_t, u_int16_t&, const u_int64_t, const u_int64_t, const bool)
/// @see If you search strings: findCharacterString()
/// @see If you search cards: findCard()
u_int64_t findNumber(const std::string& line, const u_int32_t lineInd, u_int16_t index, const u_int64_t min = 0, const u_int64_t max = -1, const bool acceptColonEnd = false) noexcept {
    u_int64_t result;
    findNumber(line, result, lineInd, index, min, max, acceptColonEnd);
    return result;
}

/// @brief Gets the string representation for a card (format: [CSHD][2-9TJQKA] or R for random card)
/// @param line The line that contains the card
/// @param lineInd The index of the line in the file
/// @param index The index of the character in the line that begins the card representation (will be changed to the end of the found string)
/// @param drawnCards The array of drawn cards to check if the card is already drawn
/// @param drawnCardCount The length of the drawnCards array
/// @return The card that is represented by the string or an invalid placeholder card(0,0) if the card is random
/// @exception Guarantee No-throw
/// @note The index, drawnCardCount and drawnCards are changed in place
/// @note Does not search for a representation. You need to provide the index of the first character of the card
/// @note If the card is random, the function will return an invalid placeholder card (0,0) that needs to be randomized later
/// @note That is done to avoid randomly generating a card that should be drawn later (you have to randomize all cards at the end)
/// @note If there is no valid card representation, the program will exit
/// @note If the card is already drawn, the program will exit
/// @see If you search strings: findCharacterString()
/// @see If you search numbers: findNumber()
Card findCard(const std::string& line, const u_int32_t lineInd, u_int16_t& index, Card drawnCards[], u_int8_t& drawnCardCount) noexcept {
    if (line[index] == 'R') {
        index++;
        // return an invalid placeholder card (we need to randomize it later)
        return Card{.rank = 0, .suit = 0};
    }
    // valid suit characters in the right order (D = 0, H = 1, S = 2, C = 3)
    const std::string validSuitChars = "DHSC";
    u_int8_t suit = 0;
    for (; suit < 4; suit++) {
        if (line[index] == validSuitChars[suit]) break;
        if (suit == 3) invalidCharFound(lineInd, index, line[index]);  // last suit got declined
    }
    index++;
    // valid rank characters in the right order
    const std::string validRankChars = "23456789TJQKA";
    u_int8_t rank = 2;  // 2 is the first valid rank
    for (; rank < 15; rank++) {
        if (line[index] == validRankChars[rank - 2]) break;
        if (rank == 14) invalidCharFound(lineInd, index, line[index]);  // last rank got declined
    }
    index++;
    Card card{.rank = rank, .suit = suit};
    // check if the card is already drawn
    if (std::find(drawnCards, &drawnCards[drawnCardCount], card) != &drawnCards[drawnCardCount]) {
        PLOG_ERROR << "Card already drawn in line " << lineInd << " at position " << index;
        std::cerr << "Card already drawn in line " << lineInd << " at position " << index << std::endl;
        exit(1);
    }
    // update the drawn cards
    drawnCards[drawnCardCount++] = card;
    return card;
}
