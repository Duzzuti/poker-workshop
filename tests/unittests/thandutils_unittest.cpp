#include <gtest/gtest.h>

#include "hand_utils.h"

TEST(THandUtils, handIndex) {
    // iterates through all card combinations where r1 >= r2 and through all suit combinations
    // checks if the index correct for all combinations and if the back-converted name is also correct
    u_int8_t expectedInd = 0;
    const std::string rankNames = "23456789TJQKA";
    for (u_int8_t r1 = 2; r1 < 15; r1++) {
        for (u_int8_t r2 = 2; r2 <= r1; r2++) {
            if (!(r1 == 2 && r2 == 2)) expectedInd++;
            for (u_int8_t s1 = 0; s1 < 4; s1++) {
                for (u_int8_t s2 = 0; s2 < 4; s2++) {
                    if (r1 == r2 && s1 == s2) continue;  // skip same cards (impossible hand)
                    std::pair<Card, Card> cards = {Card{r1, s1}, Card{r2, s2}};
                    std::pair<Card, Card> cards2 = {Card{r2, s2}, Card{r1, s1}};
                    std::pair<Card, Card> cards3 = {Card{r1, s2}, Card{r2, s1}};
                    std::pair<Card, Card> cards4 = {Card{r2, s1}, Card{r1, s2}};
                    u_int8_t index = HandUtils::getHandIndex(cards);
                    EXPECT_EQ(index, HandUtils::getHandIndex(cards2));
                    EXPECT_EQ(index, HandUtils::getHandIndex(cards3));
                    EXPECT_EQ(index, HandUtils::getHandIndex(cards4));
                    EXPECT_EQ(index, expectedInd);
                    std::string name = HandUtils::getHandName(index);
                    EXPECT_EQ(name, std::string() + rankNames[r1 - 2] + rankNames[r2 - 2]);
                }
            }
        }
    }
    EXPECT_EQ(expectedInd, 90);
}