#include "deck.h"

#include <gtest/gtest.h>

#include <algorithm>

TEST(Deck, Constructor) {
    Deck deck;
    std::vector<Card> cards;
    for (u_int8_t suit = 0; suit < 4; suit++) {
        for (u_int8_t rank = 2; rank < 15; rank++) {
            cards.push_back(Card{rank, suit});
        }
    }
    std::reverse(cards.begin(), cards.end());
    EXPECT_EQ(cards.size(), 52);
    for (u_int8_t ind = 0; ind < cards.size(); ind++) {
        Card card = deck.draw();
        EXPECT_EQ(card.rank, cards[ind].rank);
        EXPECT_EQ(card.suit, cards[ind].suit);
        EXPECT_EQ(card, cards[ind]);
    }
    EXPECT_THROW(deck.draw(), std::logic_error);
}

TEST(Deck, getRandomCardExcept) {
    Deck deck;
    std::vector<Card> cards;
    for (u_int8_t suit = 0; suit < 4; suit++) {
        for (u_int8_t rank = 2; rank < 15; rank++) {
            cards.push_back(Card{rank, suit});
        }
    }
    EXPECT_EQ(cards.size(), 52);
}