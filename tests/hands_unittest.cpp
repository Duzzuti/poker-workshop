#include <gtest/gtest.h>
#include <algorithm>

#include "hand_strengths.h"

const constexpr u_int64_t ITERATIONS = 1000;

std::vector<u_int8_t> getSortedRanks(const std::vector<Card>::iterator& begin, const std::vector<Card>::iterator& end) noexcept {
    std::vector<u_int8_t> ranks;
    ranks.reserve(end - begin);
    for(std::vector<Card>::iterator i = begin; i < end; i++){
        ranks.push_back(i->rank);
    }
    std::sort(ranks.begin(), ranks.end());
    std::reverse(ranks.begin(), ranks.end());
    return ranks;
}

TEST(HandStrengths, RoyalFlush) {
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
        for(u_int8_t suit = 0; suit < 4; suit++){
            std::vector<Card> cards{Card{14, suit}, Card{13, suit}, Card{12, suit}, Card{11, suit}, Card{10, suit}};
            cards.push_back(Deck::getRandomCardExcept(cards));
            cards.push_back(Deck::getRandomCardExcept(cards));
            for(u_int8_t iter = 0; iter < 20; iter++){
                EXPECT_EQ(cards.size(), 7);
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
                for(u_int8_t iter = 0; iter < 20; iter++){
                    EXPECT_EQ(cards.size(), 7);
                    std::random_shuffle(cards.begin(), cards.end());
                    HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                    EXPECT_EQ(hs.handkind, HandKinds::STRAIGHT_FLUSH);
                    EXPECT_EQ(hs.rankStrength, +rank);
                }
            }
        }
    }
}

TEST(HandStrengths, FourOfAKind) {
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
        for(u_int8_t rank = 2; rank < 15; rank++){
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            cards.push_back(Deck::getRandomCardExcept(cards));
            cards.push_back(Deck::getRandomCardExcept(cards));
            cards.push_back(Deck::getRandomCardExcept(cards));
            u_int32_t rankStrength = rank*2*2*2*2 + std::max<u_int8_t>(std::max<u_int8_t>(cards[4].rank, cards[5].rank), cards[6].rank);
            for(u_int8_t iter = 0; iter < 20; iter++){
                EXPECT_EQ(cards.size(), 7);
                std::random_shuffle(cards.begin(), cards.end());
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                EXPECT_EQ(hs.handkind, HandKinds::FOUR_OF_A_KIND);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, FullHouse) {
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
        for(u_int8_t rank = 2; rank < 15; rank++){
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            u_int8_t ranks[15] = {0};
            // should discard FOUR_OF_A_KINDy (flush is not possible)
            std::random_shuffle(cards.begin(), cards.end());
            cards.pop_back();   // select one triple
            cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank}));
            ranks[cards.back().rank]++;
            cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank}));
            ranks[cards.back().rank]++;
            cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank}));
            ranks[cards.back().rank]++;

            // make sure that there is at least one other pair
            Card card = Deck::getRandomCardExcept(cards, -1, {rank});
            ranks[card.rank]++;
            while(std::count(ranks, ranks + 15, 2) == 0 && std::count(ranks, ranks + 15, 3) == 0){
                // draw new card if there is no pair or triple found
                ranks[card.rank]--;
                card = Deck::getRandomCardExcept(cards, -1, {rank});
                ranks[card.rank]++;
            }
            cards.push_back(card);

            // calculate rankStrength
            u_int32_t rankStrength = 0;
            for(u_int8_t j = 14; j >= 2; j--){
                if(j == rank){
                    if((rankStrength & 0x000000F0) == 0)
                        rankStrength += rank*2*2*2*2;
                    else if((rankStrength & 0x0000000F) == 0)
                        rankStrength += rank;
                }else if(ranks[j] == 2 && ((rankStrength & 0x0000000F) == 0))
                    rankStrength += j;
                else if(ranks[j] == 3 && ((rankStrength & 0x000000F0) == 0))
                    rankStrength += j*2*2*2*2;
                else if(ranks[j] == 3 && ((rankStrength & 0x000000F0) != 0) && ((rankStrength & 0x0000000F) == 0))
                    rankStrength += j;
                else if(((rankStrength & 0x000000F0) != 0) && ((rankStrength & 0x0000000F) != 0))
                    break;
            }

            
            for(u_int8_t iter = 0; iter < 20; iter++){
                EXPECT_EQ(cards.size(), 7);
                std::random_shuffle(cards.begin(), cards.end());
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                EXPECT_EQ(hs.handkind, HandKinds::FULL_HOUSE);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, ThreeOfAKind) {
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
        for(u_int8_t rank = 2; rank < 15; rank++){
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            // avoid pairs (full house)
            std::vector<u_int8_t> ranks{rank};
            // should discard STRAIGHTy, FLUSHy, FULL_HOUSEy, FOUR_OF_A_KINDy, STRAIGHT_FLUSHy, ROYAL_FLUSHy
            std::random_shuffle(cards.begin(), cards.end());
            Card popedCard = cards.back();
            cards.pop_back();
            cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            ranks.push_back(cards.back().rank);
            cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            ranks.push_back(cards.back().rank);
            cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            ranks.push_back(cards.back().rank);

            // avoid straight
            std::sort(ranks.begin(), ranks.end());
            if(ranks[0] == ranks[3] - 3){
                ranks.push_back(ranks[0] - 1);
                ranks.push_back(ranks[3] + 1);
            }else if(ranks[0] == ranks[3] - 4){
                if(ranks[0] != ranks[1] - 1)
                    ranks.push_back(ranks[0] + 1);
                else if(ranks[1] != ranks[2] - 1)
                    ranks.push_back(ranks[1] + 1);
                else
                    ranks.push_back(ranks[2] + 1);
            }

            // avoid flush
            if(cards[3].suit == cards[4].suit && cards[4].suit == cards[5].suit && cards[5].suit != popedCard.suit)
                cards.push_back(Deck::getRandomCardExcept({}, cards[3].suit, ranks));
            else
                cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            
            ranks = getSortedRanks(cards.begin() + 3, cards.end());
            u_int32_t rankStrength = rank*2*2*2*2*2*2*2*2 + ranks[0]*2*2*2*2 + ranks[1];
            for(u_int8_t iter = 0; iter < 20; iter++){
                EXPECT_EQ(cards.size(), 7);
                std::random_shuffle(cards.begin(), cards.end());
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                EXPECT_EQ(hs.handkind, HandKinds::THREE_OF_A_KIND);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}