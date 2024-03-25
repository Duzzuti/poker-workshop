#pragma once
#include "deck.h"
#include "enums.h"

/// @brief Represents a full action that a player can take in their turn
/// @see Actions
struct Action {
    /// @brief The type of action that the player wants to take
    Actions action;
    /// @brief If the action requires a bet, this is the amount of chips that the player wants to bet
    u_int64_t bet = 0;
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
    /// @brief The number of players that are all-in in the round
    u_int8_t numAllInPlayers;
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
    u_int32_t gameWins[MAX_PLAYERS];
    /// @brief The number of round wins for each player (side pot wins included)
    u_int32_t chipWins[MAX_PLAYERS];
    /// @brief The number of chips won for each player (side pot wins included)
    u_int64_t chipWinsAmount[MAX_PLAYERS];
    /// @brief The number of chips that each player has
    u_int64_t playerChips[MAX_PLAYERS];
};

/// @brief Contains all Data for a game set (multiple games)
/// @see GameData
/// @see RoundData
/// @see BetRoundData
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
     * @exception Guarantee No-throw
     */
    u_int8_t getNextPlayerPos(u_int8_t playerPos, const bool considerFolded) const noexcept {
        do {
            playerPos = (playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[playerPos] || (considerFolded && this->roundData.playerFolded[playerPos]));
        return playerPos;
    }

    /// @brief Skips to the next active player
    /// @exception Guarantee No-throw
    void nextActivePlayer() noexcept {
        do {
            this->betRoundData.playerPos = (this->betRoundData.playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[this->betRoundData.playerPos] || this->roundData.playerFolded[this->betRoundData.playerPos] || this->getChips() == 0);
    }

    /// @brief Tries to skip to the next active player. If no active player is found, the method does nothing
    /// @exception Guarantee No-throw
    void tryNextActivePlayer() noexcept {
        const u_int8_t playerPos = this->betRoundData.playerPos;
        do {
            this->betRoundData.playerPos = (this->betRoundData.playerPos + 1) % this->numPlayers;
        } while (playerPos != this->betRoundData.playerPos &&
                 (this->gameData.playerOut[this->betRoundData.playerPos] || this->roundData.playerFolded[this->betRoundData.playerPos] || this->getChips() == 0));
    }

    /// @brief Skips to the next active or all-in player
    /// @exception Guarantee No-throw
    void nextActiveOrAllInPlayer() noexcept {
        do {
            this->betRoundData.playerPos = (this->betRoundData.playerPos + 1) % this->numPlayers;
        } while (this->gameData.playerOut[this->betRoundData.playerPos] || this->roundData.playerFolded[this->betRoundData.playerPos]);
    }

    /** @brief Selects the dealer position for the next round
     * @param firstRound True if the next round is the first round of the game
     * @exception Guarantee No-throw
     * @note For the first round the dealer position is always 0
     * @note For all other rounds the dealer position is the next active player position after the current dealer position
     */
    constexpr void selectDealer(const bool firstRound) noexcept {
        if (firstRound) {
            this->roundData.dealerPos = 0;
        } else {
            do {
                this->roundData.dealerPos = (this->roundData.dealerPos + 1) % this->numPlayers;
            } while (this->gameData.playerOut[this->roundData.dealerPos]);
        }
    }
    /** @brief Removes chips from the current player but only if he has at least 1 chip left after the removal
     * @param chips The number of chips to remove
     * @return True if the chips could be removed
     * @exception Guarantee No-throw
     * @see For all in use removeChipsAllIn()
     */
    bool removeChips(const u_int64_t chips) noexcept {
        if (this->gameData.playerChips[this->betRoundData.playerPos] < chips + 1) return false;
        this->gameData.playerChips[this->betRoundData.playerPos] -= chips;
        return true;
    }
    /** @brief Removes all chips from the current player
     * @exception Guarantee No-throw
     * @see For normal removal use removeChips()
     */
    void removeChipsAllIn() noexcept { this->gameData.playerChips[this->betRoundData.playerPos] = 0; }

    /** @brief Gets the number of chips that the current player has
     * @return Number of chips that the current player has
     * @exception Guarantee No-throw
     */
    u_int64_t getChips() const noexcept { return this->gameData.playerChips[this->betRoundData.playerPos]; }

    /** @brief Gets the number of chips that the current player has to add to match the current bet
     * @return Number of chips that the current player has to add to match the current bet
     * @exception Guarantee No-throw
     */
    u_int64_t getCallAdd() const noexcept { return this->betRoundData.currentBet - this->betRoundData.playerBets[this->betRoundData.playerPos]; }

    /** @brief Gets the number of chips that the current player has to add to raise to a bet
     * @param bet The bet that the player wants to raise to
     * @return Number of chips that the current player has to add to raise the current bet
     * @exception Guarantee No-throw
     */
    u_int64_t getRaiseAdd(const u_int64_t bet) const noexcept { return bet - this->betRoundData.playerBets[this->betRoundData.playerPos]; }

    /// @brief Prints all data to the console
    /// @exception Guarantee No-throw
    void print() const noexcept {
        std::cout << "********************** BASIC DATA **********************" << std::endl;
        std::cout << "numPlayers: " << +this->numPlayers << std::endl;
        std::cout << "********************** GAME DATA ***********************" << std::endl;
        std::cout << "numNonOutPlayers: " << +this->gameData.numNonOutPlayers << std::endl;
        std::cout << "playerOut: ";
        for (u_int8_t i = 0; i < this->numPlayers; i++) std::cout << +i << "::" << (this->gameData.playerOut[i] ? "true" : "false") << " ";
        std::cout << std::endl;
        std::cout << "playerChips: ";
        for (u_int8_t i = 0; i < this->numPlayers; i++) std::cout << +i << "::" << this->gameData.playerChips[i] << " ";
        std::cout << std::endl;
        std::cout << "Game wins: ";
        for (u_int8_t i = 0; i < this->numPlayers; i++) std::cout << +i << "::" << this->gameData.gameWins[i] << " ";
        std::cout << std::endl;
        std::cout << "Chip wins: ";
        for (u_int8_t i = 0; i < this->numPlayers; i++) std::cout << +i << "::" << this->gameData.chipWins[i] << " ";
        std::cout << std::endl;
        std::cout << "Chip wins amount: ";
        for (u_int8_t i = 0; i < this->numPlayers; i++) std::cout << +i << "::" << this->gameData.chipWinsAmount[i] << " ";
        std::cout << std::endl;
        std::cout << "********************** ROUND DATA **********************" << std::endl;
        std::cout << "smallBlind: " << this->roundData.smallBlind << std::endl;
        std::cout << "bigBlind: " << this->roundData.bigBlind << std::endl;
        std::cout << "addBlind: " << this->roundData.addBlind << std::endl;
        std::cout << "dealerPos: " << +this->roundData.dealerPos << std::endl;
        std::cout << "smallBlindPos: " << +this->roundData.smallBlindPos << std::endl;
        std::cout << "bigBlindPos: " << +this->roundData.bigBlindPos << std::endl;
        std::cout << "pot: " << this->roundData.pot << std::endl;
        std::cout << "numActivePlayers: " << +this->roundData.numActivePlayers << std::endl;
        std::cout << "playerFolded: ";
        for (u_int8_t i = 0; i < this->numPlayers; i++) std::cout << +i << "::" << (this->roundData.playerFolded[i] ? "true" : "false") << " ";
        std::cout << std::endl;
        std::cout << "communityCards: ";
        u_int8_t communityCardsCount = this->roundData.betRoundState == BetRoundState::PREFLOP ? 0
                                       : this->roundData.betRoundState == BetRoundState::FLOP  ? 3
                                       : this->roundData.betRoundState == BetRoundState::TURN  ? 4
                                                                                               : 5;
        for (u_int8_t i = 0; i < communityCardsCount; i++) std::cout << this->roundData.communityCards[i].toString() << " ";
        std::cout << std::endl;
        std::cout << "result: " << EnumToString::enumToString(this->roundData.result) << std::endl;
        std::cout << "betRoundState: " << EnumToString::enumToString(this->roundData.betRoundState) << std::endl;
        std::cout << "********************** BET ROUND DATA ******************" << std::endl;
        std::cout << "playerPos: " << +this->betRoundData.playerPos << std::endl;
        std::cout << "currentBet: " << this->betRoundData.currentBet << std::endl;
        std::cout << "minimumRaise: " << this->betRoundData.minimumRaise << std::endl;
        std::cout << "playerBets: ";
        for (u_int8_t i = 0; i < this->numPlayers; i++) std::cout << +i << "::" << this->betRoundData.playerBets[i] << " ";
        std::cout << std::endl;
        std::cout << "********************** END OF DATA *********************" << std::endl;
    }
};