#pragma once
#include <iostream>

#include "logger.h"

/// @brief constexpr strlen implementation
/// @param str string to get length of
/// @return length of string
/// @exception Guarantee No-throw
constexpr std::size_t constexpr_strlen(const char* str) {
    std::size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
};

/// @brief The maximum character length of a string that represents an u_int64_t
const constexpr u_int8_t LEN_UINT64 = 20;
/// @brief The number of cards in a deck
const constexpr u_int8_t CARD_NUM = 52;
/// @brief The number of bytes needed for a string that represents a card
const constexpr u_int8_t CARD_STR_LEN = 5;
/// @brief An array of all card ranks as characters
const constexpr char CARD_RANKS[] = "23456789TJQKA";
/// @brief An array of all card suits as strings (unicode symbols)
const constexpr char* SUIT_SYMBOLS[] = {
    u8"\xE2\x99\xA2",  // Diamonds
    u8"\xE2\x99\xA1",  // Hearts
    u8"\xE2\x99\xA0",  // Spades
    u8"\xE2\x99\xA3"   // Clubs
};
/// @brief The maximum number of players allowed in a game (affects the memory usage of the program)
const constexpr u_int8_t MAX_PLAYERS = 10;
/// @brief The maximum amount of chips in the game (MAX_CHIPS/numPlayers is the maximum starting amount for each player)
constexpr u_int64_t MAX_CHIPS = INT64_MAX - 1;
/// @brief The maximum character length allowed for a player name
const constexpr u_int8_t MAX_PLAYER_NAME_LENGTH = 20;
/// @brief The maximum character length returned from the getName() function of a player
const constexpr u_int8_t MAX_PLAYER_GET_NAME_LENGTH = MAX_PLAYER_NAME_LENGTH + 4;
/// @brief Player string constant
const constexpr char STR_PLAYER[] = "Player ";
/// @brief The maximum character length returned from the getPlayerInfo() function of a game
const constexpr u_int16_t MAX_GET_PLAYER_INFO_LENGTH = constexpr_strlen(STR_PLAYER) + MAX_PLAYER_GET_NAME_LENGTH + 5 + 2 * LEN_UINT64;
/// @brief The maximum character length for the string that represents the winning players
const constexpr u_int16_t MAX_POT_DIST_STRING_LENGTH = (MAX_GET_PLAYER_INFO_LENGTH + 2) * MAX_PLAYERS;

/// @brief Small blind error string constant
const constexpr char STR_SMALL_BLIND_ERROR[] = "cannot fulfill small blind of ";
/// @brief Big blind error string constant
const constexpr char STR_BIG_BLIND_ERROR[] = "cannot fulfill big blind of ";
/// @brief The maximum character length for the string that represents a blind error
const constexpr u_int8_t MAX_BLIND_ERROR_LENGTH = std::max<size_t>(constexpr_strlen(STR_SMALL_BLIND_ERROR), constexpr_strlen(STR_BIG_BLIND_ERROR)) + LEN_UINT64 + 1;

/// @brief Check error string constant
const constexpr char STR_CHECK_ERROR[] = "illegally tried to check current bet of ";
/// @brief Call error string constant
const constexpr char STR_CALL_ERROR[] = "illegally tried to call current bet of ";
/// @brief Raise error string constant
const constexpr char STR_RAISE_ERROR[] = "illegally tried to raise to ";
/// @brief Bet error string constant
const constexpr char STR_BET_ERROR[] = "illegally tried to bet ";
/// @brief Action error string constant
const constexpr char STR_ACTION_ERROR[] = "illegally tried to do action: ";
/// @brief The maximum character length for the string that represents an error of type check, call, raise or bet
const constexpr u_int8_t MAX_ACTION_ERROR_LENGTH =
    std::max<size_t>(std::max<size_t>(constexpr_strlen(STR_CHECK_ERROR), constexpr_strlen(STR_CALL_ERROR)), std::max<size_t>(constexpr_strlen(STR_RAISE_ERROR), constexpr_strlen(STR_BET_ERROR))) +
    LEN_UINT64 + 1;
/// @brief The maximum character length for the string that represents an error of type raise or action
const constexpr u_int8_t MAX_ACTION_ERROR_LENGTH_ONLY_RAISE = std::max<size_t>(constexpr_strlen(STR_RAISE_ERROR), constexpr_strlen(STR_ACTION_ERROR)) + LEN_UINT64 + 1;

/**
 * @brief Sets parameters for the game
 * @note A Config is a set of parameters that are used to initialize a game.
 * @note It is passed to the Game constructor.
 */
class Config {
   public:
    /// @brief Starting chips for each player
    const u_int64_t startingChips;
    /// @brief Small blind amount
    /// @note Big blind is always twice the small blind
    const u_int64_t smallBlind;
    /// @brief Increase blind for amount every time the dealer is again at position 0
    /// @note Used to avoid infinite games
    const u_int64_t addBlindPerDealer0;
    /// @brief Number of games to play
    const u_int64_t numGames;
    /// @brief Number of players in the game
    const u_int8_t numPlayers;
    /// @brief Shuffle players at the start of each round
    const bool shufflePlayers;
    /// @brief Maximum number of rounds per game
    /// @note If the rounds exceed this number, the game will end and the player with the most chips will win
    /// @note Negative means no limit
    const int16_t maxRounds;

    /// @brief Create a Config object with the given parameters
    /// @param games Number of games to play
    /// @param players Number of players in the game
    /// @param chips Starting chips for each player
    /// @param small Small blind amount
    /// @param addBlind Increase blind for amount every time the dealer is again at position 0
    /// @exception Guarantee Strong
    /// @throws std::invalid_argument if the parameters are invalid
    /// @note Big blind is always twice the small blind
    /// @note AddBlind is used to avoid infinite games
    /// @see MAX_PLAYERS for the maximum number of players
    /// @see MAX_CHIPS for the maximum amount of chips
    Config(const u_int16_t games, const u_int8_t players, const u_int64_t chips, const u_int64_t small, const u_int64_t addBlind, const bool shuffle = true, const int16_t maxRounds = -1)
        : startingChips(chips), smallBlind(small), addBlindPerDealer0(addBlind), numGames(games), numPlayers(players), shufflePlayers(shuffle), maxRounds(maxRounds) {
        if (this->numPlayers < 2 || this->numPlayers > MAX_PLAYERS) {
            PLOG_FATAL << "Invalid number of players: " << this->numPlayers << " (min: 2, max: " << MAX_PLAYERS << ")";
            throw std::invalid_argument("Invalid number of players");
        }
        if (this->startingChips * this->numPlayers > MAX_CHIPS) {
            PLOG_FATAL << "Too many chips: " << this->startingChips * this->numPlayers << " (max: " << MAX_CHIPS << ")";
            throw std::invalid_argument("Too many chips");
        }
        if (this->startingChips == 0) {
            PLOG_FATAL << "Starting chips must be greater than 0";
            throw std::invalid_argument("Starting chips must be greater than 0");
        }
    }
};

/// @brief An example Config object
class BaseConfig : public Config {
   public:
    /// @brief generates a Config object with default values
    BaseConfig() : Config(10000, 5, 1000, 10, 1) {}
};
