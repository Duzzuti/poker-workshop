#pragma once

#include "hand_strengths.h"

class HandUtils{
   public:
    /// @brief Constructor clears all arrays
    /// @param winnerAdd The amount to add to the win stat if the hand is the only winner
    /// @param splitAdd The amount to add to the win stat if the hand is a split winner
    /// @param totalAdd The amount to add to the total stat for each occurrence
    /// @exception Guarantee No-throw
    /// @note The totalAdd should be greater or equal than the maximum of winnerAdd and splitAdd to keep a meaningful total count
    /// @note winnerAdd and splitAdd are used to balance the effects of winning and splitting
    /// @note if splitAdd is 0, a split is treated as a loss, if winnerAdd is twice the value of splitAdd, a split is treated as a half win, etc.
    HandUtils(const u_int8_t winnerAdd, const u_int8_t splitAdd, const u_int8_t totalAdd) noexcept : winnerAdd(winnerAdd), splitAdd(splitAdd), totalAdd(totalAdd) {
        std::memset(this->handsSuited, 0, sizeof(this->handsSuited));
        std::memset(this->handsUnsuited, 0, sizeof(this->handsUnsuited));
        std::memset(this->handsSuitedTotal, 0, sizeof(this->handsSuitedTotal));
        std::memset(this->handsUnsuitedTotal, 0, sizeof(this->handsUnsuitedTotal));
    };

    /// @brief Get the internal array index for a hand
    /// @param playerCards The player's hand cards
    /// @return The index of the hand in the internal arrays
    /// @exception Guarantee No-throw
    /// @note The index is calculated as r1+r2-4+(r1-2)*(r1-3)/2 with r1 >= r2
    /// @note The index is needed to map the player hands to the internal arrays (91 different options) (save memory and time)
    /// @note There are hands that are "equal" like 2,3 and 3,2 
    /// @note The suit is ignored for the index, the caller has to check if the hand is suited or not
    /// @note With this in mind, there are 91 different hands (13+12+11+...+1)
    /// @see getHandName() to get the name of a hand by its index
    static constexpr u_int8_t getHandIndex(const std::pair<Card, Card> playerCards) noexcept;
    
    /// @brief Get the name of a hand by its index
    /// @param handIndex The (array) index of the hand
    /// @return The name of the hand as a string in the format "XY" where X and Y are the ranks of the cards (X >= Y)
    /// @exception Guarantee No-throw
    /// @note The name is calculated by inverting r1+r2-4+(r1-2)*(r1-3)/2 with r1 >= r2 and r1, r2 in [2, 14]
    /// @note The return array has 4 elements, the first two are the ranks + you can add a "s" or "o" to indicate suited or offsuited + null terminator
    /// @note The caller has to know if the hand is suited or not (and add the suit to the name if needed)
    /// @see getHandIndex() to get the index of a hand by its cards
    static constexpr unsigned char* getHandName(int8_t handIndex) noexcept;
    
    /// @brief Evaluate the hands of the players and update the internal arrays
    /// @param communityCards The community cards on the table (5 cards)
    /// @param playerCards The players hand cards (number of players = array length with 2 cards each)
    /// @param players The number of players
    /// @exception Guarantee No-throw
    /// @note The function calculates the hand strength of each player and compares them to find the winners and splits
    /// @note The function then updates the internal arrays with the count for wins and split and the total count for each hand
    /// @note The function does not check for impossible hands like multiple same cards
    /// @note There are also a few impossible suited hands in the arrays like AAs but to remove them would be more complex (there are just 0 occurrences of them)
    /// @see The hands are mapped to the internal arrays with the getHandIndex() function
    /// @see The function addWinners() is used to update the internal arrays
    void evaluateHands(const Card communityCards[], const std::pair<Card, Card> playerCards[], const u_int8_t players) noexcept;
    
    /// @brief Write the results to a file in csv format
    /// @param filename The name (path) of the file to write to
    /// @param players The number of players
    /// @param newFile If true, the file is created or overwritten, otherwise the results are appended
    /// @exception Guarantee No-throw
    /// @note The function writes the results for each hand in the internal arrays to the file
    /// @note It writes Player number, Hand index, Suited (true, false), Wins and Split count, Total, Wins/Total, Wins/Total*Players
    /// @note The last two are used to rank the hands by their strength
    /// @note Wins/Total is the win rate of the hand and Wins/Total*Players is the normalized win rate (1 is average win rate, <1 is under average, >1 is above average)
    void writeResults(const std::string& filename, const u_int8_t players, const bool newFile = true) const noexcept;

   private:
    /// @brief Add the data to the internal arrays
    /// @param playerCards The players hand cards
    /// @param winners The indices of the winners
    /// @param numWinners The number of winners
    /// @param players The number of players
    /// @exception Guarantee No-throw
    /// @note The function adds the data to the internal arrays based on the winners and the players hands
    /// @note The function uses the winnerAdd and splitAdd values to balance the effects of winning and splitting
    /// @note The function uses the totalAdd value to add to the total count for each occurring hand
    void addWinners(const std::pair<Card, Card> playerCards[], const u_int8_t winners[], const u_int8_t numWinners, const u_int8_t players) noexcept;


    /// @brief The amount to add to the win stat if the hand is the only winner
    const u_int8_t winnerAdd;

    /// @brief The amount to add to the win stat if the hand is a split winner
    const u_int8_t splitAdd;

    /// @brief The amount to add to the total stat for each occurrence
    const u_int8_t totalAdd;

    /// @brief The array for win stats of each suited hand
    /// @note The index is calculated with getHandIndex()
    u_int32_t handsSuited[91];

    /// @brief The array for win stats of each unsuited hand
    /// @note The index is calculated with getHandIndex()
    u_int32_t handsUnsuited[91];

    /// @brief The array for total stats of each suited hand
    /// @note The index is calculated with getHandIndex()
    u_int32_t handsSuitedTotal[91];

    /// @brief The array for total stats of each unsuited hand
    /// @note The index is calculated with getHandIndex()
    u_int32_t handsUnsuitedTotal[91];

    /// @brief The array for the ranks of the cards (lookup array)
    static constexpr unsigned char ranks[14] = "23456789TJQKA";     // + null terminator
    
    /// @brief Static member that is used to return the hand name
    static unsigned char name[4];
};