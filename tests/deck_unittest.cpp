#include "deck.h"

#include <gtest/gtest.h>

#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <list>

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

TEST(Deck, shuffle){
    for(u_int64_t i = 0; i < 1; i++){
        Deck deck{};
        Deck deck2{};
        EXPECT_EQ(deck, deck2);
        deck.shuffle();
        EXPECT_NE(deck, deck2);
        deck2.shuffle();
        EXPECT_NE(deck, deck2);
        // check if all cards are still in deck
        std::list<Card> cards;
        for (u_int8_t suit = 0; suit < 4; suit++) {
            for (u_int8_t rank = 2; rank < 15; rank++) {
                cards.push_back(Card{rank, suit});
            }
        }
        for (u_int8_t ind = 0; ind < 52; ind++)
            cards.remove(deck.draw());

        EXPECT_EQ(cards.size(), 0);
    }
    // check if the deck gets actually shuffled with entropy
    // count how often each card is at each position
    std::unordered_map<u_int8_t, u_int64_t[52]> bytesPerPos;
    for(u_int8_t ind = 0; ind < 52; ind++){
        for(u_int8_t ind2 = 0; ind2 < 52; ind2++){
            bytesPerPos[ind][ind2] = 0;
        }
    }
    double distiters = 100000;
    for(u_int64_t ind = 0; ind < distiters; ind++){
        Deck deck3{};
        deck3.shuffle();
        for(u_int8_t ind2 = 0; ind2 < 52; ind2++){
            Card card = deck3.draw();
            bytesPerPos[ind2][card.suit*13 + card.rank - 2]++;
        }
    }
    // check if some values are missing
    for(u_int8_t ind = 0; ind < 52; ind++){
        for(u_int8_t ind2 = 0; ind2 < 52; ind2++){
            EXPECT_NE(bytesPerPos[ind][ind2], 0);
        }
    }
    std::cout << "EntropyTarget: " << log2(52) << std::endl;
    for(u_int8_t ind = 0; ind < 52; ind++){
        double entropy = 0;
        for(u_int8_t ind2 = 0; ind2 < 52; ind2++){
            double cardProb = bytesPerPos[ind][ind2] / distiters;
            entropy += cardProb * log2(cardProb);
        }
        std::cout << "Entropy for pos " << +ind << ": " << -entropy << std::endl;
        EXPECT_NEAR(-entropy, log2(52), 0.001);
    }
}

TEST(Deck, Perfshuffle) {
    Deck deck;
    for (u_int64_t ind = 0; ind < 1000000; ind++) {
        deck.shuffle();
    }
}

TEST(Deck, Perfdraw) {
    for (u_int64_t ind = 0; ind < 1000000 / 52; ind++) {
        Deck deck;
        for(u_int8_t ind2 = 0; ind2 < 52; ind2++)
            deck.draw();
    }
}

TEST(Deck, PerfrandomCardExcept) {
    for (u_int64_t ind = 0; ind < 1000000; ind++) {
        Deck::getRandomCardExcept({}, -1, {});
    }
}

TEST(Deck, PerfrandomCardExceptCardsWith) {
    for (u_int64_t ind = 0; ind < 1000000; ind++) {
        Deck::getRandomCardExceptCardsWith({}, -1, -1);
    }
}