#pragma once
#include <iostream>

#include "config.h"
#include "deck.h"

/// @brief Represents a type of action that a player has to take in their turn
enum class Actions {
    /// @brief The player wants to discard their hand and leave the round
    FOLD,
    /// @brief The player wants to match the current bet
    CALL,
    /// @brief The player wants to increase the current bet
    RAISE,
    /// @brief The player wants to pass their turn
    CHECK,
    /// @brief The player wants to open the betting round with a bet
    BET,
};

/// @brief Represents a card suit
enum CardSuits {
    /// @brief Diamonds suit
    DIAMONDS = 0,
    /// @brief Hearts suit
    HEARTS,
    /// @brief Spades suit
    SPADES,
    /// @brief Clubs suit
    CLUBS,
};

/// @brief Represents a 5 cards poker hand type
enum HandKinds {
    /// @brief Used as a placeholder for an invalid or undefined hand
    NO_HAND = 0,
    /// @brief The hand does not contain any valuable combination
    HIGH_CARD,
    /// @brief The hand contains a single pair
    PAIR,
    /// @brief The hand contains two pairs
    TWO_PAIR,
    /// @brief The hand contains three cards of the same rank
    THREE_OF_A_KIND,
    /// @brief The hand contains five cards of ascending rank
    STRAIGHT,
    /// @brief The hand contains five cards of the same suit
    FLUSH,
    /// @brief The hand contains three cards of the same rank and another pair
    FULL_HOUSE,
    /// @brief The hand contains four cards of the same rank
    FOUR_OF_A_KIND,
    /// @brief The hand contains five cards of ascending rank and the same suit
    STRAIGHT_FLUSH,
    /// @brief The hand contains A, K, Q, J and 10 of the same suit
    ROYAL_FLUSH,
};

/// @brief Represents the current state of a betting round (preflop, flop, turn, river)
enum class BetRoundState {
    /// @brief There are no community cards yet
    PREFLOP,
    /// @brief The first three community cards are on the table
    FLOP,
    /// @brief The fourth community card is on the table
    TURN,
    /// @brief The fifth community card is on the table
    RIVER,
};

/// @brief Represents a full action that a player can take in their turn
struct Action {
    /// @brief The type of action that the player wants to take
    Actions action;
    /// @brief If the action requires a bet, this is the amount of chips that the player wants to bet
    u_int64_t bet = 0;
};

/// @brief Represents the state of the whole round
enum class OutEnum {
    /// @brief The round is still ongoing
    ROUND_CONTINUE,
    /// @brief The GAME is won by a player
    GAME_WON,
    /// @brief The ROUND and pot is won by a player
    ROUND_WON,
};

/// @brief Contains the data for a single bet round (preflop, flop, turn, river)
struct BetRoundData {
    /// @brief Position of the player that is currently playing
    u_int8_t playerPos;
    /// @brief Current bet in the bet round
    u_int64_t currentBet;
    /// @brief Minimum raise allowed for the current player
    u_int64_t minimumRaise;
    /// @brief Number of chips betted for each player in the current round
    u_int64_t playerBets[MAX_PLAYERS];
};

/// @brief Contains the data for a single round (until the pot is won)
struct RoundData {
    /// @brief The current small blind
    u_int64_t smallBlind;
    /// @brief The current big blind
    u_int64_t bigBlind;
    /// @brief The amount of chips that are added to the small blind every time the dealer is again at position 0
    u_int64_t addBlind;
    /// @brief The position of the dealer
    u_int8_t dealerPos;
    /// @brief The position of the small blind
    u_int8_t smallBlindPos;
    /// @brief The position of the big blind
    u_int8_t bigBlindPos;
    /// @brief The current pot
    u_int64_t pot;
    /// @brief The number of players that are still in the round
    u_int8_t numActivePlayers;
    /// @brief True if the player folded
    bool playerFolded[MAX_PLAYERS];
    /// @brief The community cards, betRoundState needs to be checked to know how many of them are valid
    Card communityCards[5];
    /// @brief Whats the state of the round (continue, round won, game won)
    OutEnum result;
    /// @brief Whats the state of the bet round (preflop, flop, turn, river)
    BetRoundState betRoundState;
};

/// @brief Contains the data for a single game (until only one player is left)
struct GameData {
    /// @brief The number of players that are still in the game
    u_int8_t numNonOutPlayers;
    /// @brief True if the player is out of the game
    bool playerOut[MAX_PLAYERS];
    /// @brief The number of game wins for each player
    u_int32_t winners[MAX_PLAYERS];
    /// @brief The number of chips that each player has
    u_int64_t playerChips[MAX_PLAYERS];
};

/// @brief Contains all Data for a game set (multiple games)
struct Data {
    /// @brief The number of players competing in the games
    u_int8_t numPlayers;
    /// @brief The data for the current game
    GameData gameData;
    /// @brief The data for the current round
    RoundData roundData;
    /// @brief The data for the current bet round
    BetRoundData betRoundData;

    /** @brief Gets the next active player position without changing it
     * @param playerPos The current player position
     * @param considerFolded Should folded players also be skipped?
     * @return The next active player position
     * @exception No-throw
     */
    u_int8_t getNextPlayerPos(u_int8_t playerPos, const bool considerFolded) const noexcept {
        do {
            playerPos = (playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[playerPos] || (considerFolded && this->roundData.playerFolded[playerPos]));
        return playerPos;
    }

    /// @brief Skips to the next player who is not out of the game
    /// @see If you want to skip folded players use nextActivePlayer()
    /// @exception No-throw
    void nextPlayer() noexcept {
        do {
            this->betRoundData.playerPos = (this->betRoundData.playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[this->betRoundData.playerPos]);
    }

    /// @brief Skips to the next active player
    /// @see If you want to skip only players who are out of the game use nextPlayer()
    /// @exception No-throw
    void nextActivePlayer() noexcept {
        do {
            this->betRoundData.playerPos = (this->betRoundData.playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[this->betRoundData.playerPos] || this->roundData.playerFolded[this->betRoundData.playerPos]);
    }

    /** @brief Selects the dealer position for the next round
     * @param firstRound True if the next round is the first round of the game
     * @exception No-throw
     * @note For the first round the dealer position is always 0
     * @note For all other rounds the dealer position is the next active player position after the current dealer position
     */
    void selectDealer(const bool firstRound) noexcept {
        if (firstRound) {
            this->roundData.dealerPos = 0;
        } else {
            do {
                this->roundData.dealerPos = (this->roundData.dealerPos + 1) % this->numPlayers;
            } while (this->gameData.playerOut[this->roundData.dealerPos]);
        }
    }
    /** @brief Removes chips from the current player but only if he has at least 1 chip left after the removal
     * @see For all in use removeChipsAllIn()
     * @param chips The number of chips to remove
     * @return True if the chips could be removed
     * @exception No-throw
     */
    bool removeChips(const u_int64_t chips) noexcept {
        if (this->gameData.playerChips[this->betRoundData.playerPos] < chips + 1) return false;
        this->gameData.playerChips[this->betRoundData.playerPos] -= chips;
        return true;
    }
    /** @brief Removes all chips from the current player
     * @see For normal removal use removeChips()
     * @exception No-throw
     */
    void removeChipsAllIn() noexcept { this->gameData.playerChips[this->betRoundData.playerPos] = 0; }

    /** @brief Gets the number of chips that the current player has
     * @return Number of chips that the current player has
     * @exception No-throw
     */
    u_int64_t getChips() const noexcept { return this->gameData.playerChips[this->betRoundData.playerPos]; }

    /** @brief Gets the number of chips that the current player has to add to match the current bet
     * @return Number of chips that the current player has to add to match the current bet
     * @exception No-throw
     */
    u_int64_t getCallAdd() const noexcept { return this->betRoundData.currentBet - this->betRoundData.playerBets[this->betRoundData.playerPos]; }

    /** @brief Gets the number of chips that the current player has to add to raise to a bet
     * @param bet The bet that the player wants to raise to
     * @return Number of chips that the current player has to add to raise the current bet
     * @exception No-throw
     */
    u_int64_t getRaiseAdd(const u_int64_t bet) const noexcept { return bet - this->betRoundData.playerBets[this->betRoundData.playerPos]; }
};