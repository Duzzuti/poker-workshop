#include "hand_strengths.h"

#include <algorithm>

void HandStrengths::getHandStrengths(Player* const players[], const Data& data, HandStrengths result[]) noexcept {
    for (u_int8_t i = 0; i < data.numPlayers; i++) {
        if (!(data.gameData.playerOut[i] || data.roundData.playerFolded[i])) {
            result[i] = getHandStrength(players[i]->getHand(), data.roundData.communityCards);
        }
    }
}

HandStrengths HandStrengths::getHandStrength(const std::pair<Card, Card>& hand, const Card community[]) noexcept {
    u_int8_t suits[4] = {0, 0, 0, 0};
    u_int8_t ranks[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
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

    u_int8_t rank11 = 0;
    u_int8_t rank12 = 0;
    u_int8_t rank13 = 0;
    u_int8_t rank14 = 0;
    u_int8_t rank15 = 0;
    u_int8_t rank21 = 0;
    u_int8_t rank22 = 0;
    u_int8_t rank23 = 0;
    u_int8_t rank31 = 0;
    u_int8_t rank32 = 0;
    u_int8_t rank4 = 0;
    int8_t flush_suit = -1;

    for (u_int8_t j = 0; j < 4; j++) {
        if (suits[j] >= 5) {
            // flush
            flush_suit = j;
            break;
        }
    }

    for (int8_t j = 12; j >= 0; j--) {
        switch (ranks[j]) {
            case 4:
                rank4 = j + 2;
                break;
            case 3:
                if (rank31 == 0)
                    rank31 = j + 2;
                else if (rank32 == 0)
                    rank32 = j + 2;
                break;
            case 2:
                if (rank21 == 0)
                    rank21 = j + 2;
                else if (rank22 == 0)
                    rank22 = j + 2;
                else if (rank23 == 0)
                    rank23 = j + 2;
                break;
            case 1:
                if (rank11 == 0)
                    rank11 = j + 2;
                else if (rank12 == 0)
                    rank12 = j + 2;
                else if (rank13 == 0)
                    rank13 = j + 2;
                else if (rank14 == 0)
                    rank14 = j + 2;
                else if (rank15 == 0)
                    rank15 = j + 2;
                break;
            default:
                break;
        }
    }

    // available hands: high card, pair, two pair, three of a kind, straight, flush, full house, four of a kind, straight flush, royal flush

    if (flush_suit != -1) {
        // flush
        // available hands: flush, full house, four of a kind, straight flush, royal flush
        Card flush_cards[7];
        u_int8_t flush_length = 0;

        if (hand.first.suit == flush_suit) flush_cards[flush_length++] = hand.first;
        if (hand.second.suit == flush_suit) flush_cards[flush_length++] = hand.second;

        for (u_int8_t j = 0; j < 5; j++) {
            if (community[j].suit == flush_suit) flush_cards[flush_length++] = community[j];
        }
        std::sort(flush_cards, &flush_cards[flush_length], [](const Card& card1, const Card& card2) { return card1.rank > card2.rank; });
        for (u_int8_t j = 0; j < flush_length - 4; j++) {
            if (flush_cards[j].rank == flush_cards[j + 4].rank + 4) {
                if (flush_cards[j].rank == 14) {
                    // royal flush
                    return HandStrengths{HandKinds::ROYAL_FLUSH, 0};
                } else {
                    // straight flush
                    return HandStrengths{HandKinds::STRAIGHT_FLUSH, flush_cards[j].rank};
                }
            }
        }
        if (rank4 != 0) {
            // four of a kind
            return HandStrengths{HandKinds::FOUR_OF_A_KIND, (u_int32_t)(rank4 << 4) | std::max<u_int8_t>(std::max<u_int8_t>(rank11, rank21), rank31)};
        } else if (rank31 != 0 && (rank32 != 0 || rank21 != 0)) {
            // full house
            return HandStrengths{HandKinds::FULL_HOUSE, (u_int32_t)(rank31 << 4) | std::max<u_int8_t>(rank32, rank21)};
        } else {
            // flush
            return HandStrengths{HandKinds::FLUSH, getRankStrength(flush_cards, 5)};
        }
    } else {
        // no flush
        // available hands: high card, pair, two pair, three of a kind, straight, full house, four of a kind
        if (rank4 != 0) {
            // four of a kind
            return HandStrengths{HandKinds::FOUR_OF_A_KIND, (u_int32_t)(rank4 << 4) | std::max<u_int8_t>(std::max<u_int8_t>(rank11, rank21), rank31)};
        }
        if (rank31 != 0 && (rank32 != 0 || rank21 != 0)) {
            // full house
            return HandStrengths{HandKinds::FULL_HOUSE, (u_int32_t)(rank31 << 4) | std::max<u_int8_t>(rank32, rank21)};
        }
        // available hands: high card, pair, two pair, three of a kind, straight
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
        } else {
            // high card
            return HandStrengths{HandKinds::HIGH_CARD, (u_int32_t)(rank11 << 16) | (u_int32_t)(rank12 << 12) | (u_int32_t)(rank13 << 8) | (u_int32_t)(rank14 << 4) | rank15};
        }
    }
}

u_int32_t HandStrengths::getRankStrength(const Card sortedCards[], const u_int8_t num) noexcept {
    u_int32_t rankStrength = 0;
    for (u_int8_t i = 0; i < num; i++) {
        rankStrength |= sortedCards[i].rank << 4 * (num - i - 1);
    }
    return rankStrength;
}

bool operator>(const HandStrengths& lhs, const HandStrengths& rhs) noexcept {
    if (lhs.handkind > rhs.handkind) {
        return true;
    } else if (lhs.handkind == rhs.handkind) {
        return lhs.rankStrength > rhs.rankStrength;
    }
    return false;
}

bool operator==(const HandStrengths& lhs, const HandStrengths& rhs) noexcept { return lhs.handkind == rhs.handkind && lhs.rankStrength == rhs.rankStrength; }
