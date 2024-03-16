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
    constexpr HandStrengths(const HandKinds handkind, const u_int32_t rankStrength) noexcept : handkind(handkind), rankStrength(rankStrength){};

    /// @brief Default constructor
    /// @exception Guarantee No-throw
    /// @note Initializes handkind to NO_HAND and rankStrength to 0
    constexpr HandStrengths() noexcept : handkind(HandKinds::NO_HAND), rankStrength(0){};

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
    static constexpr void getHandStrengths(const std::unique_ptr<Player> players[], const Data& data, HandStrengths result[]) noexcept {
        for (u_int8_t i = 0; i < data.numPlayers; i++) {
            if (!(data.roundData.playerFolded[i] || data.gameData.playerOut[i])) {
                // only calculate hand strength if player is still in the game
                result[i] = getHandStrength(players[i]->getHand(), data.roundData.communityCards);
            }
        }
    };

    /// @brief Gets the HandStrengths of a hand + community cards combination
    /// @param hand The hand of the player (2 Cards)
    /// @param community The community cards (5 Cards) (has to have length 5)
    /// @return The HandStrengths of the hand + community cards combination
    /// @exception Guarantee No-throw
    /// @exception UnexpectedBehaviour If the size of community is not 5
    /// @see Card
    static constexpr HandStrengths getHandStrength(const std::pair<Card, Card>& hand, const Card community[]) noexcept {
        // saves the number of cards of each suit and rank
        u_int8_t suits[4] = {0, 0, 0, 0};
        u_int8_t ranks[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        // add cards to suits and ranks
        suits[hand.first.suit]++;
        suits[hand.second.suit]++;
        for (u_int8_t j = 0; j < 5; j++) {
            suits[community[j].suit]++;
        }
        ranks[hand.first.rank - 2]++;
        ranks[hand.second.rank - 2]++;
        for (u_int8_t j = 0; j < 5; j++) {
            ranks[community[j].rank - 2]++;
        }

        // saves the ranks for the different kinds
        // 0 means not found, else rank (2-14)
        u_int8_t rank11 = 0;  // rank of highest card
        u_int8_t rank12 = 0;  // rank of second highest card
        u_int8_t rank13 = 0;  // rank of third highest card
        u_int8_t rank14 = 0;  // rank of fourth highest card
        u_int8_t rank15 = 0;  // rank of fifth highest card
        u_int8_t rank21 = 0;  // rank of highest pair
        u_int8_t rank22 = 0;  // rank of second highest pair
        u_int8_t rank23 = 0;  // rank of third highest pair
        u_int8_t rank31 = 0;  // rank of highest three of a kind
        u_int8_t rank32 = 0;  // rank of second highest three of a kind
        u_int8_t rank4 = 0;   // rank of four of a kind

        int8_t flush_suit = -1;  // -1 (no flush) or suit of flush (0-3)

        for (u_int8_t j = 0; j < 4; j++) {
            if (suits[j] >= 5) {
                // flush found with suit j
                flush_suit = j;
                break;
            }
        }

        // find pairs, three of a kind and four of a kind from highest to lowest rank
        for (int8_t j = 12; j >= 0; j--) {
            switch (ranks[j]) {
                case 4:
                    // four of a kind (highest)
                    rank4 = j + 2;
                    break;
                case 3:
                    // three of a kind
                    if (rank31 == 0)  // first three of a kind (highest)
                        rank31 = j + 2;
                    else  // second three of a kind (second highest)
                        rank32 = j + 2;
                    break;
                case 2:
                    // pair
                    if (rank21 == 0)  // first pair (highest)
                        rank21 = j + 2;
                    else if (rank22 == 0)  // second pair (second highest)
                        rank22 = j + 2;
                    else  // third pair (third highest)
                        rank23 = j + 2;
                    break;
                case 1:
                    // single card
                    if (rank11 == 0)  // first single card (highest)
                        rank11 = j + 2;
                    else if (rank12 == 0)  // second single card (second highest)
                        rank12 = j + 2;
                    else if (rank13 == 0)  // third single card (third highest)
                        rank13 = j + 2;
                    else if (rank14 == 0)  // fourth single card (fourth highest)
                        rank14 = j + 2;
                    else if (rank15 == 0)  // fifth single card (fifth highest)
                        rank15 = j + 2;
                    break;
                default:
                    break;
            }
        }

        // possible hands: high card, pair, two pair, three of a kind, straight, flush, full house, four of a kind, straight flush, royal flush

        if (flush_suit != -1) {
            // flush
            // possible hands: flush, full house, four of a kind, straight flush, royal flush
            if (rank4 != 0) {
                // four of a kind, cannot be a straight flush or royal flush (because 4 cards of the same rank are needed for four of a kind)
                return HandStrengths{HandKinds::FOUR_OF_A_KIND, (u_int32_t)(rank4 << 4) | std::max<u_int8_t>(std::max<u_int8_t>(rank11, rank21), rank31)};
            } else if (rank31 != 0 && (rank32 != 0 || rank21 != 0)) {
                // full house, cannot be a straight flush or royal flush (because 5 cards of two ranks are needed for a full house)
                return HandStrengths{HandKinds::FULL_HOUSE, (u_int32_t)(rank31 << 4) | std::max<u_int8_t>(rank32, rank21)};
            }
            // possible hands: flush, straight flush, royal flush

            // cards that are part of the flush
            Card flush_cards[7]{};
            // number of cards that are part of the flush
            u_int8_t flush_length = 0;

            // check hand cards
            if (hand.first.suit == flush_suit) flush_cards[flush_length++] = hand.first;
            if (hand.second.suit == flush_suit) flush_cards[flush_length++] = hand.second;

            // check community cards
            for (u_int8_t j = 0; j < 5; j++) {
                if (community[j].suit == flush_suit) flush_cards[flush_length++] = community[j];
            }
            // sort flush cards from highest to lowest rank
            std::sort(flush_cards, &flush_cards[flush_length], [](const Card& card1, const Card& card2) { return card1.rank > card2.rank; });
            for (u_int8_t j = 0; j < flush_length - 4; j++) {
                // check if straight flush
                if (flush_cards[j].rank == flush_cards[j + 4].rank + 4) {
                    if (flush_cards[j].rank == 14) {
                        // royal flush, because straight flush with ace as highest card
                        return HandStrengths{HandKinds::ROYAL_FLUSH, 0};
                    } else {
                        // straight flush
                        return HandStrengths{HandKinds::STRAIGHT_FLUSH, flush_cards[j].rank};
                    }
                }
            }
            // flush
            return HandStrengths{HandKinds::FLUSH, getRankStrength(flush_cards, 5)};
        } else {
            // no flush
            // possible hands: high card, pair, two pair, three of a kind, straight, full house, four of a kind
            if (rank4 != 0) {
                // four of a kind
                return HandStrengths{HandKinds::FOUR_OF_A_KIND, (u_int32_t)(rank4 << 4) | std::max<u_int8_t>(std::max<u_int8_t>(rank11, rank21), rank31)};
            } else if (rank31 != 0 && (rank32 != 0 || rank21 != 0)) {
                // full house
                return HandStrengths{HandKinds::FULL_HOUSE, (u_int32_t)(rank31 << 4) | std::max<u_int8_t>(rank32, rank21)};
            }
            // possible hands: high card, pair, two pair, three of a kind, straight
            for (u_int8_t j = 12; j >= 4; j--) {
                if (ranks[j] != 0 && ranks[j - 1] != 0 && ranks[j - 2] != 0 && ranks[j - 3] != 0 && ranks[j - 4] != 0) {
                    // straight
                    return HandStrengths{HandKinds::STRAIGHT, j + 2U};
                }
            }
            if (rank31 != 0) {
                // three of a kind
                return HandStrengths{HandKinds::THREE_OF_A_KIND, (u_int32_t)(rank31 << 8) | (u_int32_t)(rank11 << 4) | rank12};
            } else if (rank21 != 0 && rank22 != 0) {
                // two pair
                return HandStrengths{HandKinds::TWO_PAIR, (u_int32_t)(rank21 << 8) | (u_int32_t)(rank22 << 4) | std::max<u_int8_t>(rank11, rank23)};
            } else if (rank21 != 0) {
                // pair
                return HandStrengths{HandKinds::PAIR, (u_int32_t)(rank21 << 12) | (u_int32_t)(rank11 << 8) | (u_int32_t)(rank12 << 4) | rank13};
            }
            // high card
            return HandStrengths{HandKinds::HIGH_CARD, (u_int32_t)(rank11 << 16) | (u_int32_t)(rank12 << 12) | (u_int32_t)(rank13 << 8) | (u_int32_t)(rank14 << 4) | rank15};
        }
    };

    /// @brief Gets the rankStrength of a sorted list of cards
    /// @param sortedCards Sorted by importance (descending)
    /// @param num The number of cards in sortedCards
    /// @return The rankStrength of the sortedCards
    /// @exception Guarantee No-throw
    /// @exception UnexpectedBehaviour If the size of sortedCards is not num
    /// @see Card
    static constexpr u_int32_t getRankStrength(const Card sortedCards[], const u_int8_t num) noexcept {
        u_int32_t rankStrength = 0;
        for (u_int8_t i = 0; i < num; i++) {
            // shift the rank of the card to the left by 4 * (num-1, num-2, ..., 0) bits and add it to rankStrength
            rankStrength |= sortedCards[i].rank << 4 * (num - i - 1);
        }
        return rankStrength;
    };

    /// @brief Checks if the HandStrengths is stronger than another HandStrengths
    /// @param lhs First HandStrengths
    /// @param rhs Second HandStrengths
    /// @return True if lhs is stronger than rhs, false otherwise
    /// @exception Guarantee No-throw
    /// @note First compares the handkind, then the rankStrength as a tiebreaker
    friend constexpr bool operator>(const HandStrengths& lhs, const HandStrengths& rhs) noexcept {
        // compare hand kinds
        if (lhs.handkind > rhs.handkind) {
            return true;
        } else if (lhs.handkind == rhs.handkind) {
            // compare rank strengths
            return lhs.rankStrength > rhs.rankStrength;
        }
        return false;
    };

    /// @brief Checks if the HandStrengths is equal to another HandStrengths
    /// @param lhs First HandStrengths
    /// @param rhs Second HandStrengths
    /// @return True if lhs is equal to rhs, false otherwise
    /// @exception Guarantee No-throw
    /// @note Compare the handkind and the rankStrength
    friend constexpr bool operator==(const HandStrengths& lhs, const HandStrengths& rhs) noexcept { return lhs.handkind == rhs.handkind && lhs.rankStrength == rhs.rankStrength; };
};