#pragma once

#include "player.h"

/// @brief Contains functionalities to determine the strength of a hand
class HandStrengths {
   public:
    /// @brief The kind of the hand
    /// @note A handkind is a combination of the cards in the hand and the community cards
    /// @example pair, straight, flush, full house, etc.
    /// @see HandKinds
    HandKinds handkind;

    /// @brief The strength of a handkind
    /// @note Determined by the rank of the cards in the handkind
    /// @example pair of 2s is weaker than pair of 3s
    u_int32_t rankStrength;

    /// @brief Default constructor
    /// @exception Guarantee No-throw
    /// @param handkind handkind of the hand
    /// @param rankStrength rankStrength of the hand
    /// @see HandKinds
    HandStrengths(HandKinds handkind, u_int32_t rankStrength) noexcept : handkind(handkind), rankStrength(rankStrength){};

    /// @brief Default constructor
    /// @exception Guarantee No-throw
    /// @note Initializes handkind to NO_HAND and rankStrength to 0
    HandStrengths() noexcept : handkind(HandKinds::NO_HAND), rankStrength(0){};

    /// @brief Calculates the HandStrengths for every player in the game
    /// @param players The players array
    /// @param data The data of the game
    /// @param result The array where the result is stored (has to be at least the size of players)
    /// @exception Guarantee No-throw
    /// @exception UnexpectedBehaviour If the size of result is smaller than the size of players
    /// @note The result is stored in the result array
    /// @note Every result[i] is the HandStrengths of players[i]
    /// @note Players who are not active are ignored
    /// @see Data
    /// @see Player
    static void getHandStrengths(const std::unique_ptr<Player> players[], const Data& data, HandStrengths result[]) noexcept;

    /// @brief Gets the HandStrengths of a hand + community cards combination
    /// @param hand The hand of the player (2 Cards)
    /// @param community The community cards (5 Cards) (has to have length 5)
    /// @return The HandStrengths of the hand + community cards combination
    /// @exception Guarantee No-throw
    /// @exception UnexpectedBehaviour If the size of community is not 5
    /// @see Card
    static HandStrengths getHandStrength(const std::pair<Card, Card>& hand, const Card community[]) noexcept;

    /// @brief Gets the rankStrength of a sorted list of cards
    /// @param sortedCards Sorted by importance (descending)
    /// @param num The number of cards in sortedCards
    /// @return The rankStrength of the sortedCards
    /// @exception Guarantee No-throw
    /// @exception UnexpectedBehaviour If the size of sortedCards is not num
    /// @see Card
    static u_int32_t getRankStrength(const Card sortedCards[], const u_int8_t num) noexcept;

    /// @brief Checks if the HandStrengths is stronger than another HandStrengths
    /// @param lhs First HandStrengths
    /// @param rhs Second HandStrengths
    /// @return True if lhs is stronger than rhs, false otherwise
    /// @exception Guarantee No-throw
    /// @note First compares the handkind, then the rankStrength as a tiebreaker
    friend bool operator>(const HandStrengths& lhs, const HandStrengths& rhs) noexcept;

    /// @brief Checks if the HandStrengths is equal to another HandStrengths
    /// @param lhs First HandStrengths
    /// @param rhs Second HandStrengths
    /// @return True if lhs is equal to rhs, false otherwise
    /// @exception Guarantee No-throw
    /// @note Compare the handkind and the rankStrength
    friend bool operator==(const HandStrengths& lhs, const HandStrengths& rhs) noexcept;
};