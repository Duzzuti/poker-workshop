#pragma once
#include <iostream>

#include "logger.h"

const constexpr u_int8_t LEN_UINT64 = 20;

const constexpr u_int8_t CARD_NUM = 52;
const constexpr u_int8_t CARD_STR_LEN = 5;
const constexpr char CARD_RANKS[] = "23456789TJQKA";
const constexpr char* SUIT_SYMBOLS[] = {
    u8"\xE2\x99\xA2",  // Diamonds
    u8"\xE2\x99\xA3",  // Hearts
    u8"\xE2\x99\xA0",  // Spades
    u8"\xE2\x99\xA1"   // Clubs
};

const constexpr u_int8_t MAX_PLAYERS = 10;
const constexpr u_int64_t MAX_CHIPS = INT64_MAX - 1;  // max chips on the whole table
const constexpr u_int8_t MAX_PLAYER_NAME_LENGTH = 20;
const constexpr u_int8_t MAX_PLAYER_GET_NAME_LENGTH = MAX_PLAYER_NAME_LENGTH + 4;  // +4 for the player number (u_int8_t) and the colon (:)
const constexpr char STR_PLAYER[] = "Player ";
const constexpr u_int16_t MAX_GET_PLAYER_INFO_LENGTH = std::strlen(STR_PLAYER) + MAX_PLAYER_GET_NAME_LENGTH + 5 + 2 * LEN_UINT64;  // 5 for "[ + ]"
const constexpr u_int16_t MAX_POT_DIST_STRING_LENGTH = (MAX_GET_PLAYER_INFO_LENGTH + 2) * MAX_PLAYERS;

const constexpr char STR_SMALL_BLIND_ERROR[] = "cannot fulfill small blind of ";
const constexpr char STR_BIG_BLIND_ERROR[] = "cannot fulfill big blind of ";
const constexpr u_int8_t MAX_BLIND_ERROR_LENGTH = std::max<size_t>(std::strlen(STR_SMALL_BLIND_ERROR), std::strlen(STR_BIG_BLIND_ERROR)) + LEN_UINT64 + 1;

const constexpr char STR_CHECK_ERROR[] = "illegally tried to check current bet of ";
const constexpr char STR_CALL_ERROR[] = "illegally tried to call current bet of ";
const constexpr char STR_RAISE_ERROR[] = "illegally tried to raise to ";
const constexpr char STR_BET_ERROR[] = "illegally tried to bet ";
const constexpr char STR_ACTION_ERROR[] = "illegally tried to do action: ";
const constexpr u_int8_t MAX_ACTION_ERROR_LENGTH =
    std::max<size_t>(std::max<size_t>(std::strlen(STR_CHECK_ERROR), std::strlen(STR_CALL_ERROR)), std::max<size_t>(std::strlen(STR_RAISE_ERROR), std::strlen(STR_BET_ERROR))) + LEN_UINT64 + 1;
const constexpr u_int8_t MAX_ACTION_ERROR_LENGTH_ONLY_RAISE = std::max<size_t>(std::strlen(STR_RAISE_ERROR), std::strlen(STR_ACTION_ERROR)) + LEN_UINT64 + 1;

class Config {
   public:
    u_int64_t startingChips;       // Starting chips for each player
    u_int64_t smallBlind;          // Small blind amount
    u_int64_t addBlindPerDealer0;  // Add blind amount every time the dealer is again at position 0
    u_int64_t numRounds;           // Number of rounds to play
    u_int8_t numPlayers;           // Number of players in the game

    // Constructor
    Config(u_int16_t rounds, u_int8_t players, u_int64_t chips, u_int64_t small, u_int64_t addBlind)
        : startingChips(chips), smallBlind(small), addBlindPerDealer0(addBlind), numRounds(rounds), numPlayers(players) {
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

class BaseConfig : public Config {
   public:
    BaseConfig() : Config(1000, 5, 1000, 10, 1) {}  // Default values
};
