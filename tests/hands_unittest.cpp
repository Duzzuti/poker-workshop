#include <gtest/gtest.h>

#include <algorithm>

#include "hand_strengths.h"

/*
every test generates random 7 cards that form the tested hand.
the tests are designed to generate every possible hand for every category.
the tests are generating random combination. You can set the ITERATIONS constant to a higher value to generate more combinations.
*/
const constexpr u_int64_t ITERATIONS = 4000;

// get last occurence of an element in an array
template <typename T>
T* rfind(T* begin, T* end, const T& value) noexcept {
    for (T* i = end - 1; i >= begin; i--) {
        if (*i == value) return i;
    }
    return end;
}

TEST(HandStrengths, RoyalFlush) {
    // generate royal flush hands and test if it is detected as royal flush
    for (u_int64_t iters = 0; iters < ITERATIONS * 8; iters++) {
        // generate royal flush for every suit
        for (u_int8_t suit = 0; suit < 4; suit++) {
            // royal flush hand
            Card cards[7]{Card{14, suit}, Card{13, suit}, Card{12, suit}, Card{11, suit}, Card{10, suit}};
            // add two random cards
            cards[5] = Deck::getRandomCardExcept(cards, 5);
            cards[6] = Deck::getRandomCardExcept(cards, 6);
            for (u_int8_t iter = 0; iter < 20; iter++) {
                std::random_shuffle(cards, &cards[7]);
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                EXPECT_EQ(hs.handkind, HandKinds::ROYAL_FLUSH);
                EXPECT_EQ(hs.rankStrength, 0);
            }
        }
    }
}

TEST(HandStrengths, StraightFlush) {
    // generate straight flush hands and test if it is detected as straight flush
    for (u_int64_t iters = 0; iters < ITERATIONS; iters++) {
        // generate straight flush for every suit
        for (u_int8_t suit = 0; suit < 4; suit++) {
            // generate every possible straight in the suit (without royal flush, where rank = 14)
            for (u_int8_t rank = 6; rank < 14; rank++) {
                Card cards[8]{Card{rank, suit}, Card{(u_int8_t)(rank - 1), suit}, Card{(u_int8_t)(rank - 2), suit}, Card{(u_int8_t)(rank - 3), suit}, Card{(u_int8_t)(rank - 4), suit}};
                // avoid that the straight gets higher than rank
                cards[7] = Card{(u_int8_t)(rank + 1), suit};    // this cards is not part of the straight and gets ignored
                cards[5] = Deck::getRandomCardExcept(cards, 8);
                cards[6] = Deck::getRandomCardExcept(cards, 8);
                for (u_int8_t iter = 0; iter < 20; iter++) {
                    std::random_shuffle(cards, &cards[7]);
                    HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                    EXPECT_EQ(hs.handkind, HandKinds::STRAIGHT_FLUSH);
                    EXPECT_EQ(hs.rankStrength, +rank);  // rank strength is the highest rank of the straight
                }
            }
        }
    }
}

TEST(HandStrengths, FourOfAKind) {
    // generate four of a kind hands and test if it is detected as four of a kind
    for (u_int64_t iters = 0; iters < ITERATIONS * 4; iters++) {
        // generate four of a kind for every rank
        for (u_int8_t rank = 2; rank < 15; rank++) {
            // there cannot be a straight flush because there are only 3 extra cards
            Card cards[7]{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            cards[4] = Deck::getRandomCardExcept(cards, 4);
            cards[5] = Deck::getRandomCardExcept(cards, 5);
            cards[6] = Deck::getRandomCardExcept(cards, 6);
            // rank strength is the rank of the four of a kind + the highest rank of the extra cards
            u_int32_t rankStrength = (rank << 4) + std::max<u_int8_t>(std::max<u_int8_t>(cards[4].rank, cards[5].rank), cards[6].rank);
            for (u_int8_t iter = 0; iter < 20; iter++) {
                std::random_shuffle(cards, &cards[7]);
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                EXPECT_EQ(hs.handkind, HandKinds::FOUR_OF_A_KIND);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, FullHouse) {
    // generate full house hands and test if it is detected as full house
    for (u_int64_t iters = 0; iters < ITERATIONS * 4; iters++) {
        // generate the triple of full house for every rank
        for (u_int8_t rank = 2; rank < 15; rank++) {
            // start with a quad and remove a random card
            Card cards[7]{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            u_int8_t ranks[15] = {0};
            // should discard FOUR_OF_A_KINDy (flush is not possible)
            std::random_shuffle(cards, &cards[4]);
            // select one triple (cards[3] gets discarded)
            // add three random cards by avoiding quads
            for (u_int8_t j = 3; j < 6; j++) {
                cards[j] = Deck::getRandomCardExcept(cards, j, -1, &rank, 1);
                ranks[cards[j].rank]++;
            }

            // make sure that there is at least one other pair or triple by also avoiding another quad
            Card card = Deck::getRandomCardExcept(cards, 6, -1, &rank, 1);
            ranks[card.rank]++;
            if (std::find(ranks + 2, ranks + 15, 4) != ranks + 15) {
                // there is another quad, we need to avoid it
                ranks[card.rank]--;
                card = Deck::getRandomCardExcept(cards, 6, -1, std::array<u_int8_t, 2>{rank, card.rank}.data(), 2);  // draw new card that does not lead to a quad
                ranks[card.rank]++;
            } else if (std::find(ranks + 2, ranks + 15, 3) == ranks + 15) {
                while (std::find(ranks + 2, ranks + 15, 2) == ranks + 15) {
                    // draw new card if there is no pair or triple found besides the rank triple
                    ranks[card.rank]--;
                    card = Deck::getRandomCardExcept(cards, 6, -1, &rank, 1);
                    ranks[card.rank]++;
                }
            }
            cards[6] = card;

            // calculate rankStrength
            // find another triple
            u_int8_t* highest3 = rfind<u_int8_t>(ranks + 2, ranks + 15, 3);
            u_int32_t rankStrength;
            if (highest3 != ranks + 15) {
                // if there is an other triple, rankStrength is the rank of the higher triple + the rank of the lower triple
                if (highest3 > ranks + rank)
                    rankStrength = ((highest3 - ranks) << 4) + rank;
                else
                    rankStrength = (rank << 4) + (highest3 - ranks);
            } else
                // if there is no other triple, rankStrength is the rank of the triple + the rank of the pair
                rankStrength = (rank << 4) + (rfind<u_int8_t>(ranks + 2, ranks + 15, 2) - ranks);

            for (u_int8_t iter = 0; iter < 20; iter++) {
                std::random_shuffle(cards, &cards[7]);
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                EXPECT_EQ(hs.handkind, HandKinds::FULL_HOUSE);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, Flush) {
    // generate flush hands and test if it is detected as flush
    for (u_int64_t iters = 0; iters < ITERATIONS * 8; iters++) {
        // generate flush for every suit
        for (u_int8_t suit = 0; suit < 4; suit++) {
            // store every choosen card and every card with the choosen suit
            Card cards[7];
            Card flushCards[7];
            // should discard FULL_HOUSEy (impossible), FOUR_OF_A_KINDy (impossible), STRAIGHT_FLUSHy, ROYAL_FLUSHy
            // we only need to discard straight flushes
            while (true) {
                // add 5 random cards with the same suit
                for (u_int8_t j = 0; j < 5; j++) {
                    cards[j] = Deck::getRandomCardExceptCardsWith(cards, j, suit);
                    flushCards[j] = cards[j];
                }
                u_int8_t flushLength = 5;
                // add 2 more random cards
                for (u_int8_t j = 5; j < 7; j++) {
                    cards[j] = Deck::getRandomCardExcept(cards, j);
                    if (cards[j].suit == suit) flushCards[flushLength++] = cards[j];
                }
                // sort the flush cards, there are no duplicates because there is only one card of each rank per suit
                std::sort(flushCards, &flushCards[flushLength], [](const Card& a, const Card& b) -> bool { return a.rank > b.rank; });
                bool breakFlag = true;  // break if there is no straight
                for (u_int8_t j = 0; j < flushLength - 4; j++) {
                    if (flushCards[j].rank == flushCards[j + 4].rank + 4) {
                        // there is a straight, continue
                        breakFlag = false;
                        break;
                    }
                }
                if (breakFlag) break;
            }

            // flushcards are sorted from highest to lowest rank
            u_int32_t rankStrength = (flushCards[0].rank << 16) + (flushCards[1].rank << 12) + (flushCards[2].rank << 8) + (flushCards[3].rank << 4) + flushCards[4].rank;

            for (u_int8_t iter = 0; iter < 20; iter++) {
                std::random_shuffle(cards, &cards[7]);
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                EXPECT_EQ(hs.handkind, HandKinds::FLUSH);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, Straight) {
    // generate straight hands and test if it is detected as straight
    for (u_int64_t iters = 0; iters < ITERATIONS * 5; iters++) {
        // generate straights for every rank (rank is the highest rank of the straight)
        for (u_int8_t rank = 6; rank < 15; rank++) {
            Card cards[7];
            // should discard FLUSHy, FULL_HOUSEy (impossible), FOUR_OF_A_KINDy (impossible), STRAIGHT_FLUSHy, ROYAL_FLUSHy
            // we only need to discard flushes
            // select random straight
            u_int8_t suits[4] = {0};
            u_int8_t cardSize = 0;
            for (u_int8_t j = rank - 4; j <= rank; j++) {
                // add random card of rank j
                Card cards1[4]{Card{j, 0}, Card{j, 1}, Card{j, 2}, Card{j, 3}};
                std::random_shuffle(cards1, &cards1[4]);
                suits[cards1[3].suit]++;
                // add card if it does not create a flush
                if (j != rank || std::find(suits, suits + 4, 5) == suits + 4)
                    cards[cardSize++] = cards1[3];
                else {
                    suits[cards1[3].suit]--;
                    j--;  // try again with the same rank
                }
            }
            // add two random cards by avoiding flush
            int8_t avoidSuit = -1;
            while (cardSize < 7) {
                cards[cardSize] = Deck::getRandomCardExcept(cards, cardSize, avoidSuit, std::array<u_int8_t, 1>{(u_int8_t)(rank + 1)}.data(), 1);  // dont spawn higher straights
                cardSize++;
                suits[cards[cardSize-1].suit]++;
                // check if there is a flush
                if (std::find(suits, suits + 4, 5) != suits + 4) {
                    avoidSuit = cards[--cardSize].suit;  // avoid this suit for the next card
                    suits[avoidSuit]--;
                }
            }
            // rank strength is the highest rank of the straight
            u_int32_t rankStrength = +rank;
            for (u_int8_t iter = 0; iter < 20; iter++) {
                std::random_shuffle(cards, &cards[7]);
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                EXPECT_EQ(hs.handkind, HandKinds::STRAIGHT);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, ThreeOfAKind) {
    // generate three of a kind hands and test if it is detected as three of a kind
    for (u_int64_t iters = 0; iters < ITERATIONS * 4; iters++) {
        // generate three of a kind for every rank
        for (u_int8_t rank = 2; rank < 15; rank++) {
            // start with a quad and remove one random cards
            Card cards[7]{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            // avoid pairs (full house)
            u_int8_t ranks[7]{rank};
            // should discard STRAIGHTy, FLUSHy, FULL_HOUSEy, FOUR_OF_A_KINDy, STRAIGHT_FLUSHy, ROYAL_FLUSHy
            std::random_shuffle(cards, &cards[4]);
            Card popedCard = cards[3];
            // cards[3] gets discarded
            // add three random cards by avoiding quads and pairs
            for (u_int8_t j = 3; j < 6; j++) {
                cards[j] = Deck::getRandomCardExcept(cards, j,  -1, ranks, j - 2);
                ranks[j - 2] = cards[j].rank;
            }

            // avoid straight
            std::sort(ranks, &ranks[4]);
            // get highest and second highest rank that is not rank
            u_int8_t highestRank;
            u_int8_t secondHighestRank;
            if (ranks[3] != rank) {
                highestRank = ranks[3];
                if (ranks[2] != rank)
                    secondHighestRank = ranks[2];
                else
                    secondHighestRank = ranks[1];
            } else {
                highestRank = ranks[2];
                secondHighestRank = ranks[1];
            }
            u_int8_t rankSize = 4;
            if (ranks[0] == ranks[3] - 3) {
                // nearly a straight, we need to avoid one rank lower or higher cards
                ranks[rankSize++] = ranks[0] - 1;
                ranks[rankSize++] = ranks[3] + 1;
            } else if (ranks[0] == ranks[3] - 4) {
                // nearly a straight, we need to avoid inbetween ranked cards
                if (ranks[0] != ranks[1] - 1)
                    // avoid rank between ranks[0] and ranks[1]
                    ranks[rankSize++] = ranks[0] + 1;
                else if (ranks[1] != ranks[2] - 1)
                    // avoid rank between ranks[1] and ranks[2]
                    ranks[rankSize++] = ranks[1] + 1;
                else
                    // avoid rank between ranks[2] and ranks[3]
                    ranks[rankSize++] = ranks[2] + 1;
            }

            // avoid flush
            if (cards[3].suit == cards[4].suit && cards[4].suit == cards[5].suit && cards[5].suit != popedCard.suit)
                // if we have 3 cards of the same suit and the poped card is not of the same suit
                // (that means that one of the triplet also has this suit), we need to avoid the same suit
                cards[6] = Deck::getRandomCardExcept({}, 0, cards[3].suit, ranks, rankSize);
            else
                cards[6] = Deck::getRandomCardExcept({}, 0, -1, ranks, rankSize);

            // correct highest and second highest rank if necessary
            if (cards[6].rank > highestRank) {
                secondHighestRank = highestRank;
                highestRank = cards[6].rank;
            } else if (cards[6].rank > secondHighestRank)
                secondHighestRank = cards[6].rank;

            // rank strength is the rank of the triplet + the highest ranks of the extra cards
            u_int32_t rankStrength = (rank << 8) + (highestRank << 4) + secondHighestRank;
            for (u_int8_t iter = 0; iter < 20; iter++) {
                std::random_shuffle(cards, &cards[7]);
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                EXPECT_EQ(hs.handkind, HandKinds::THREE_OF_A_KIND);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, TwoPair) {
    // generate two pair hands and test if it is detected as two pair
    for (u_int64_t iters = 0; iters < ITERATIONS / 3 + 1; iters++) {
        // generate two pair for every rank
        for (u_int8_t rank1 = 2; rank1 < 15; rank1++) {
            for (u_int8_t rank2 = 2; rank2 < 15; rank2++) {
                // we need two different pairs
                if (rank1 == rank2) continue;
                Card cards[7];
                u_int8_t cardSize = 0;
                u_int8_t suits[4] = {0};
                u_int32_t rankStrength = 0;
                // should discard THREE_OF_A_KINDy, STRAIGHTy, FLUSHy,
                // FULL_HOUSEy (discarded by 3ofAKind), FOUR_OF_A_KINDy (discarded by 3ofAKind), STRAIGHT_FLUSHy (discarded by flush), ROYAL_FLUSHy (discarded by flush)

                // push two pairs into cards
                for (u_int8_t rank : {rank1, rank2}) {
                    Card cards1[4]{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
                    std::random_shuffle(cards1, &cards1[4]);
                    cards[cardSize++] = cards1[3];
                    cards[cardSize++] = cards1[0];
                    suits[cards1[3].suit]++;
                    suits[cards1[0].suit]++;
                }

                while (true) {
                    // reset to two pairs
                    while (cardSize > 4) {
                        rankStrength = 0;
                        suits[cards[--cardSize].suit]--;
                    }
                    // add 3 random cards by avoiding three of a kind with rank1 and rank2
                    for (u_int8_t j = 4; j < 7; j++) {
                        cards[j] = Deck::getRandomCardExcept(cards, j, -1, std::array<u_int8_t, 2>{rank1, rank2}.data(), 2);
                        suits[cards[j].suit]++;
                    }
                    cardSize = 7;
                    // check if there is a flush (only possible if the 3 random cards are the same suit)
                    u_int8_t avoidSuit = -1;
                    if (std::find(suits, suits + 4, 5) != suits + 4) {
                        // delete last card and get a new one with a different suit
                        avoidSuit = cards[6].suit;
                        suits[avoidSuit]--;
                        cards[6] = Deck::getRandomCardExcept(cards, 6, avoidSuit, std::array<u_int8_t, 2>{rank1, rank2}.data(), 2);
                        suits[cards[6].suit]++;
                    }
                    // check if there is a triple (full house). Only possible if the 3 random cards are the same rank
                    u_int8_t tmpRanks[3] = {0, rank1, rank2};
                    if (cards[4].rank == cards[5].rank) {
                        if (cards[5].rank == cards[6].rank) {
                            // there is a triple, delete the last card and get a new one with a different rank
                            suits[cards[6].suit]--;
                            cards[6] = Deck::getRandomCardExcept(cards, 6, avoidSuit, std::array<u_int8_t, 3>{rank1, rank2, cards[5].rank}.data(), 3);
                            suits[cards[6].suit]++;
                        }
                        // there are 3 pairs
                        tmpRanks[0] = cards[4].rank;
                    } else if (cards[5].rank == cards[6].rank || cards[4].rank == cards[6].rank)
                        // there are 3 pairs
                        tmpRanks[0] = cards[6].rank;
                    // sort the pair ranks to get the highest and second highest pair for rankStrength
                    std::sort(tmpRanks, &tmpRanks[3]);
                    rankStrength = (tmpRanks[2] << 8) + (tmpRanks[1] << 4);

                    // check if there is a straight
                    u_int8_t ranks[15] = {0};
                    for (u_int8_t j = 0; j < 7; j++) ranks[cards[j].rank]++;
                    bool breakFlag = true;
                    // check for straight
                    for (u_int8_t j = 2; j < 11; j++) {
                        if (ranks[j] && ranks[j + 1] && ranks[j + 2] && ranks[j + 3] && ranks[j + 4]) {
                            breakFlag = false;
                            break;
                        }
                    }
                    if (breakFlag) {
                        // add highest available card (not included in two pair) to rankStrength
                        for (u_int8_t k = 14; k > 2; k--) {
                            if (ranks[k] && k != tmpRanks[2] && k != tmpRanks[1]) {
                                rankStrength += k;
                                break;
                            }
                        }
                        break;
                    }
                }
                for (u_int8_t iter = 0; iter < 20; iter++) {
                    std::random_shuffle(cards, &cards[7]);
                    HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                    EXPECT_EQ(hs.handkind, HandKinds::TWO_PAIR);
                    EXPECT_EQ(hs.rankStrength, rankStrength);
                }
            }
        }
    }
}

TEST(HandStrengths, Pair) {
    // generate pair hands and test if it is detected as pair
    for (u_int64_t iters = 0; iters < ITERATIONS*3; iters++) {
        for (u_int8_t rank = 2; rank < 15; rank++) {
            // start with a quad and remove two random cards
            Card cards[7]{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            u_int8_t ranks[7]{rank};  // discard three of a kind and another pair
            u_int8_t suits[4] = {0};
            // should discard TWO_PAIRy, THREE_OF_A_KINDy, STRAIGHTy, FLUSHy,
            // FULL_HOUSEy (discarded by THREE_OF_A_KIND), FOUR_OF_A_KINDy (discarded by THREE_OF_A_KIND), STRAIGHT_FLUSHy (discarded by flush), ROYAL_FLUSHy (discarded by flush)
            std::random_shuffle(cards, &cards[4]);
            // discard cards[3] and cards[2]
            // add 3 random cards by avoiding triplets and pairs
            for (u_int8_t j = 2; j < 5; j++) {
                cards[j] = Deck::getRandomCardExcept(cards, j,  -1, ranks, j - 1);
                ranks[j - 1] = cards[j].rank;
            }
            for (u_int8_t j = 0; j < 5; j++) suits[cards[j].suit]++;

            u_int8_t cardSize = 5;
            u_int8_t ranksSize = 4;
            // add the other cards while avoiding flush and straight
            while (cardSize < 7) {
                cards[cardSize] = Deck::getRandomCardExcept(cards, cardSize, -1, ranks, ranksSize);
                suits[cards[cardSize].suit]++;
                cardSize++;

                // avoid flush
                if (std::find(suits, suits + 4, 5) != suits + 4) {
                    // delete last card and get a new one with a different suit
                    u_int8_t avoidSuit = cards[--cardSize].suit;
                    suits[avoidSuit]--;
                    cards[cardSize] = Deck::getRandomCardExcept(cards, cardSize, avoidSuit, ranks, ranksSize);
                    suits[cards[cardSize].suit]++;
                    cardSize++;
                }
                ranks[ranksSize++] = cards[cardSize-1].rank;
                u_int8_t sortedRankSize = ranksSize;
                u_int8_t sortedRanks[sortedRankSize];
                std::memcpy(sortedRanks, ranks, sortedRankSize);
                std::sort(sortedRanks, &sortedRanks[sortedRankSize]);
                // we dont need to delete duplicates because there is only one pair and the rank is only in the array once
                // avoid straight
                for (u_int8_t j = 0; j < sortedRankSize - 4; j++) {
                    if (sortedRanks[j] == sortedRanks[j + 4] - 4) {
                        suits[cards[--cardSize].suit]--;
                        ranksSize--;
                        break;
                    }
                }
            }
            // remove pair rank
            std::sort(&ranks[1], &ranks[ranksSize]);  // sort ranks

            u_int32_t rankStrength = (rank << 12) + (ranks[5] << 8) + (ranks[4] << 4) + ranks[3];
            for (u_int8_t iter = 0; iter < 20; iter++) {
                std::random_shuffle(cards, &cards[7]);
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
                EXPECT_EQ(hs.handkind, HandKinds::PAIR);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, HighCard) {
    // generate high card hands and test if it is detected as high card
    for (u_int64_t iters = 0; iters < ITERATIONS * 40; iters++) {
        Card cards[7];
        u_int8_t ranks[7];
        u_int8_t suits[4] = {0};
        u_int32_t rankStrength = 0;
        // should discard PAIRy, STRAIGHTy, FLUSHy,
        // TWO_PAIRy (discarded by PAIR), THREE_OF_A_KINDy (discarded by PAIR),
        // FULL_HOUSEy (discarded by PAIR), FOUR_OF_A_KINDy (discarded by PAIR), STRAIGHT_FLUSHy (discarded by flush), ROYAL_FLUSHy (discarded by flush)

        // add the other cards while avoiding flush, straight and pair
        u_int8_t avoidSuit = -1;
        u_int8_t sortedRanks[7];
        u_int8_t cardSize = 0;
        u_int8_t ranksSize = 0;
        while (cardSize < 7) {
            cards[cardSize] = Deck::getRandomCardExcept(cards, cardSize, avoidSuit, ranks, ranksSize);
            suits[cards[cardSize++].suit]++;

            // avoid flush
            if (cardSize > 4 && std::find(suits, suits + 4, 5) != suits + 4) {
                avoidSuit = cards[--cardSize].suit;
                suits[avoidSuit]--;
                continue;
            }
            ranks[ranksSize++] = cards[cardSize-1].rank;
            if (ranksSize > 4) {
                std::memcpy(sortedRanks, ranks, ranksSize);
                std::sort(sortedRanks, &sortedRanks[ranksSize]);
                // we dont need to delete duplicates because there is no pair
                // avoid straight
                for (int8_t j = 0; j < (int8_t)ranksSize - 4; j++) {
                    if (sortedRanks[j] == sortedRanks[j + 4] - 4) {
                        suits[cards[--cardSize].suit]--;
                        ranksSize--;
                        break;
                    }
                }
            }
        }

        rankStrength = (sortedRanks[6] << 16) + (sortedRanks[5] << 12) + (sortedRanks[4] << 8) + (sortedRanks[3] << 4) + sortedRanks[2];
        for (u_int8_t iter = 0; iter < 20; iter++) {
            std::random_shuffle(cards, &cards[7]);
            HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::array<Card, 5>{cards[2], cards[3], cards[4], cards[5], cards[6]}.data());
            EXPECT_EQ(hs.handkind, HandKinds::HIGH_CARD);
            EXPECT_EQ(hs.rankStrength, rankStrength);
        }
    }
}
