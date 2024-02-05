#include <gtest/gtest.h>

#include "game_test.h"

TEST(GameTest, buildDeck) {
    // test with different number of players
    u_int8_t numPlayers[3] = {2, 4, MAX_PLAYERS};
    for (u_int8_t i = 0; i < 3; i++) {
        // set up the game and the player hands
        GameTest gameTest(Config{1, numPlayers[i], 100, 100, 1, false, false, 1});
        std::pair<Card, Card> playerHands[numPlayers[i]];
        Card outCards[52];
        u_int8_t outCardsInd = 0;
        // gets randomly cards for the players and the community
        for (u_int8_t j = 0; j < numPlayers[i]; j++) {
            Card card1 = Deck::getRandomCardExcept(outCards, outCardsInd);
            outCards[outCardsInd++] = card1;
            Card card2 = Deck::getRandomCardExcept(outCards, outCardsInd);
            outCards[outCardsInd++] = card2;
            playerHands[j] = std::pair<Card, Card>{card1, card2};
        }
        Card communityCards[5];
        for (u_int8_t j = 0; j < 5; j++) {
            communityCards[j] = Deck::getRandomCardExcept(outCards, outCardsInd);
            outCards[outCardsInd++] = communityCards[j];
        }
        // build the desired deck
        gameTest.buildDeck(playerHands, numPlayers[i], communityCards);
        // check if the deck is as expected
        for (u_int8_t j = 0; j < numPlayers[i]; j++) {
            EXPECT_EQ(gameTest.getDeck().draw(), playerHands[j].first);
            EXPECT_EQ(gameTest.getDeck().draw(), playerHands[j].second);
        }
        for (u_int8_t j = 0; j < 5; j++) {
            EXPECT_EQ(gameTest.getDeck().draw(), communityCards[j]);
        }
    }
}