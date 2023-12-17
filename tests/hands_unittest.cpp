#include <gtest/gtest.h>

#include "hand_strengths.h"

const constexpr u_int64_t ITERATIONS = 1000;

TEST(HandStrengths, RoyalFlush) {
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
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
}

TEST(HandStrengths, StraightFlush) {
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
        for(u_int8_t suit = 0; suit < 4; suit++){
            for(u_int8_t rank = 6; rank < 14; rank++){
                std::vector<Card> cards{Card{rank, suit}, Card{(u_int8_t)(rank - 1), suit}, Card{(u_int8_t)(rank - 2), suit}, Card{(u_int8_t)(rank - 3), suit}, Card{(u_int8_t)(rank - 4), suit}};
                std::vector<Card> cards2 = cards;
                cards2.push_back(Card{(u_int8_t)(rank + 1), suit});
                Card card1 = Deck::getRandomCardExcept(cards2);
                cards.push_back(card1);
                cards2.push_back(card1);
                Card card2 = Deck::getRandomCardExcept(cards2);
                cards.push_back(card2);
                for(u_int8_t iter = 0; iter < 255; iter++){
                    std::random_shuffle(cards.begin(), cards.end());
                    HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                    EXPECT_EQ(hs.handkind, HandKinds::STRAIGHT_FLUSH);
                    EXPECT_EQ(hs.rankStrength, +rank);
                }
            }
        }
    }
}