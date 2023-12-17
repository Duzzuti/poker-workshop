#include <gtest/gtest.h>

#include "hand_strengths.h"

TEST(HandStrengths, RoyalFlush) {
    for(u_int8_t suit = 0; suit < 4; suit++){
        std::vector<Card> cards{Card{14, suit}, Card{13, suit}, Card{12, suit}, Card{11, suit}, Card{10, suit}};
        Card card1 = Deck::getRandomCardExcept(cards);
        cards.push_back(card1);
        Card card2 = Deck::getRandomCardExcept(cards);
        cards.push_back(card2);
        for(u_int8_t iter = 0; iter < 255; iter++){
            std::random_shuffle(cards.begin(), cards.end());
            HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
            EXPECT_EQ(hs.handkind, HandKinds::ROYAL_FLUSH);
            EXPECT_EQ(hs.rankStrength, 0);
        }
    }
}