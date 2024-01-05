#include "pot.h"

#include <gtest/gtest.h>

TEST(PotTest, isCapped) {
    Pot pot;
    EXPECT_FALSE(pot.isCapped());
    pot.setCap(100);
    EXPECT_TRUE(pot.isCapped());
    pot.setCap(1);
    EXPECT_TRUE(pot.isCapped());
    pot.setCap(0);
    EXPECT_FALSE(pot.isCapped());
}

TEST(PotTest, addChips) {
    Pot pot;
    EXPECT_EQ(pot.getPotValue(), 0);
    u_int64_t wholeBet = 100;
    u_int64_t remaining = pot.addChips(wholeBet);
    EXPECT_EQ(pot.getPotValue(), 100);
    EXPECT_EQ(remaining, 0);
    wholeBet = 200;
    remaining = pot.addChips(wholeBet);
    EXPECT_EQ(pot.getPotValue(), 300);
    EXPECT_EQ(remaining, 0);
    pot.setCap(249);
    wholeBet = 300;
    remaining = pot.addChips(wholeBet);
    EXPECT_EQ(pot.getPotValue(), 549);
    EXPECT_EQ(remaining, 51);
    wholeBet = 100;
    remaining = pot.addChips(wholeBet);
    EXPECT_EQ(pot.getPotValue(), 649);
    EXPECT_EQ(remaining, 0);

    pot.reset();
    EXPECT_EQ(pot.getPotValue(), 0);
    wholeBet = 100;
    remaining = pot.addChips(wholeBet, 50);
    EXPECT_EQ(pot.getPotValue(), 50);
    EXPECT_EQ(remaining, 0);
    wholeBet = 200;
    remaining = pot.addChips(wholeBet, 50);
    EXPECT_EQ(pot.getPotValue(), 200);
    EXPECT_EQ(remaining, 0);
    pot.setCap(249);
    wholeBet = 300;
    remaining = pot.addChips(wholeBet, 50);
    EXPECT_EQ(pot.getPotValue(), 399);
    EXPECT_EQ(remaining, 51);
    wholeBet = 100;
    remaining = pot.addChips(wholeBet, 50);
    EXPECT_EQ(pot.getPotValue(), 449);
    EXPECT_EQ(remaining, 0);
}

TEST(PotTest, addNonContender) {
    Pot pot;
    for (u_int8_t i = 0; i < MAX_PLAYERS - 1; i++) {
        for (u_int8_t j = 0; j < MAX_PLAYERS; j++) {
            if (j >= i)
                EXPECT_TRUE(pot.isContender(j));
            else
                EXPECT_FALSE(pot.isContender(j));
        }
        if (i < MAX_PLAYERS - 2) pot.addNonContender(i);
    }
    pot.reset();
    for (int16_t i = MAX_PLAYERS - 1; i >= 1; i--) {
        for (int16_t j = 0; j < MAX_PLAYERS; j++) {
            if (j > i)
                EXPECT_FALSE(pot.isContender(j));
            else
                EXPECT_TRUE(pot.isContender(j));
        }
        if (i >= 2) pot.addNonContender(i);
    }
}
