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
    for(u_int64_t iters = 0; iters < ITERATIONS*8; iters++){
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
    for(u_int64_t iters = 0; iters < ITERATIONS*2; iters++){
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

TEST(HandStrengths, Flush) {
    for(u_int64_t iters = 0; iters < ITERATIONS/30 + 1; iters++){
        for(u_int8_t suit = 0; suit < 4; suit++){
            std::vector<Card> cards{};
            std::vector<Card> flushCards{};
            cards.reserve(7);
            flushCards.reserve(7);
            // should discard FULL_HOUSEy (impossible), FOUR_OF_A_KINDy (impossible), STRAIGHT_FLUSHy, ROYAL_FLUSHy
            // we only need to discard straight flushes
            while(true){
                cards.clear();
                flushCards.clear();
                for(u_int8_t j = 0; j < 7; j++){
                    cards.push_back(Deck::getRandomCardExcept(cards));
                    if(cards.back().suit == suit)
                        flushCards.push_back(cards.back());
                }
                if(flushCards.size() < 5)
                    continue;
                std::sort(flushCards.begin(), flushCards.end(), [](const Card& a, const Card& b) -> bool {return a.rank > b.rank;});
                bool breakFlag = true;
                for(u_int8_t j = 0; j < flushCards.size() - 4; j++){
                    if(flushCards[j].rank == flushCards[j + 4].rank + 4){
                        breakFlag = false;
                        break;
                    }
                }
                if(breakFlag)
                    break;
            }
            
            u_int32_t rankStrength = 
                flushCards[4].rank + 
                flushCards[3].rank*2*2*2*2 + 
                flushCards[2].rank*2*2*2*2*2*2*2*2 + 
                flushCards[1].rank*2*2*2*2*2*2*2*2*2*2*2*2 + 
                flushCards[0].rank*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2;

            for(u_int8_t iter = 0; iter < 20; iter++){
                EXPECT_EQ(cards.size(), 7);
                std::random_shuffle(cards.begin(), cards.end());
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                EXPECT_EQ(hs.handkind, HandKinds::FLUSH);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, Straight) {
    for(u_int64_t iters = 0; iters < ITERATIONS*5; iters++){
        for(u_int8_t rank = 6; rank < 14; rank++){
            std::vector<Card> cards{};
            cards.reserve(7);
            // should discard FLUSHy, FULL_HOUSEy (impossible), FOUR_OF_A_KINDy (impossible), STRAIGHT_FLUSHy, ROYAL_FLUSHy
            // we only need to discard flushes
            // select random straight
            u_int8_t suits[4] = {0};
            for(u_int8_t j = rank - 4; j <= rank; j++){
                std::vector<Card> cards1{Card{j, 0}, Card{j, 1}, Card{j, 2}, Card{j, 3}};
                std::random_shuffle(cards1.begin(), cards1.end());
                suits[cards1.back().suit]++;
                if(std::count(suits, suits + 4, 5) == 0)
                    cards.push_back(cards1.back());
                else{
                    suits[cards1.back().suit]--;
                    j--;
                }
            }
            // add two random cards
            for(u_int8_t j = 0; j < 2; j++){
                while(true){
                    cards.push_back(Deck::getRandomCardExcept(cards, -1, {(u_int8_t)(rank + 1)}));  // dont spawn higher straights
                    suits[cards.back().suit]++;
                    if(std::count(suits, suits + 4, 5) == 0)
                        break;
                    else{
                        suits[cards.back().suit]--;
                        cards.pop_back();
                    }
                }
            }
            
            u_int32_t rankStrength = +rank;
            for(u_int8_t iter = 0; iter < 20; iter++){
                EXPECT_EQ(cards.size(), 7);
                std::random_shuffle(cards.begin(), cards.end());
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                EXPECT_EQ(hs.handkind, HandKinds::STRAIGHT);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, ThreeOfAKind) {
    for(u_int64_t iters = 0; iters < ITERATIONS*2; iters++){
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

TEST(HandStrengths, TwoPair) {
    for(u_int64_t iters = 0; iters < ITERATIONS/6 + 1; iters++){
        for(u_int8_t rank1 = 2; rank1 < 15; rank1++){
            for(u_int8_t rank2 = 2; rank2 < 15; rank2++){
                if(rank1 == rank2)
                    continue;
                std::vector<Card> cards{};
                cards.reserve(7);
                u_int8_t suits[4] = {0};
                u_int32_t rankStrength = 0;
                // should discard THREE_OF_A_KINDy, STRAIGHTy, FLUSHy, 
                // FULL_HOUSEy (discarded by 3ofAKind), FOUR_OF_A_KINDy (discarded by 3ofAKind), STRAIGHT_FLUSHy (discarded by flush), ROYAL_FLUSHy (discarded by flush)
                
                // push two pairs into cards
                std::vector<Card> cards1{Card{rank1, 0}, Card{rank1, 1}, Card{rank1, 2}, Card{rank1, 3}};
                std::vector<Card> cards2{Card{rank2, 0}, Card{rank2, 1}, Card{rank2, 2}, Card{rank2, 3}};
                std::random_shuffle(cards1.begin(), cards1.end());
                std::random_shuffle(cards2.begin(), cards2.end());
                cards.push_back(cards1.back());
                cards.push_back(cards1[0]);
                cards.push_back(cards2.back());
                cards.push_back(cards2[0]);
                suits[cards[0].suit]++;
                suits[cards[1].suit]++;
                suits[cards[2].suit]++;
                suits[cards[3].suit]++;

                while(true){
                    // reset to two pairs
                    while(cards.size() > 4){
                        rankStrength = 0;
                        suits[cards.back().suit]--;
                        cards.pop_back();
                    }
                    // add random cards by avoiding three of a kind with rank1 and rank2
                    cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank1, rank2}));
                    cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank1, rank2}));
                    cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank1, rank2}));
                    suits[cards[4].suit]++;
                    suits[cards[5].suit]++;
                    suits[cards[6].suit]++;
                    // check if there is a triple (full house). Only possible if the 3 random cards are the same rank
                    std::vector<u_int8_t> tmpRanks{0, rank1, rank2};
                    if(cards[4].rank == cards[5].rank){
                        if(cards[5].rank == cards[6].rank)
                            continue;
                        tmpRanks = {rank1, rank2, cards[4].rank};
                    }
                    if(cards[5].rank == cards[6].rank || cards[4].rank == cards[6].rank)
                        tmpRanks = {rank1, rank2, cards[6].rank};
                    std::sort(tmpRanks.begin(), tmpRanks.end());
                    rankStrength = tmpRanks[2]*2*2*2*2*2*2*2*2 + tmpRanks[1]*2*2*2*2;
                    // check if there is a flush
                    if(std::count(suits, suits + 4, 5) == 1)
                        continue;
                    // check if there is a straight
                    std::vector<Card> sortedCards = cards;
                    std::sort(sortedCards.begin(), sortedCards.end(), [](const Card& a, const Card& b) -> bool {return a.rank > b.rank;});
                    // remove duplicates to check for straight
                    std::vector<Card> sortedCardSet = {sortedCards[0]};
                    for(u_int8_t j = 1; j < sortedCards.size(); j++){
                        if(sortedCards[j - 1].rank != sortedCards[j].rank)
                            sortedCardSet.push_back(sortedCards[j]);
                    }
                    bool breakFlag = true;
                    // check for straight
                    for(u_int8_t j = 0; j < sortedCardSet.size() - 4; j++){
                        if(sortedCardSet[j].rank == sortedCardSet[j + 4].rank + 4){
                            breakFlag = false;
                            break;
                        }
                    }
                    if(breakFlag){
                        // add highest available card (not included in two pair) to rankStrength
                        for(u_int8_t k = 0; k < 5; k++){
                            if(sortedCardSet[k].rank != tmpRanks[2] && sortedCardSet[k].rank != tmpRanks[1]){
                                rankStrength += sortedCardSet[k].rank;
                                break;
                            }
                        }
                        break;
                    }
                }
                for(u_int8_t iter = 0; iter < 20; iter++){
                    EXPECT_EQ(cards.size(), 7);
                    std::random_shuffle(cards.begin(), cards.end());
                    HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                    if(hs.handkind != HandKinds::TWO_PAIR){
                        std::cout << "cards: " << cards[0].toString() << " " << cards[1].toString() << " " << cards[2].toString() << " " << cards[3].toString() << " " << cards[4].toString() << " " << cards[5].toString() << " " << cards[6].toString() << std::endl;
                    }
                    EXPECT_EQ(hs.handkind, HandKinds::TWO_PAIR);
                    EXPECT_EQ(hs.rankStrength, rankStrength);
                }
            }
        }
    }
}

TEST(HandStrengths, Pair) {
    for(u_int64_t iters = 0; iters < ITERATIONS/2*3; iters++){
        for(u_int8_t rank = 2; rank < 15; rank++){
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            std::vector<u_int8_t> ranks{rank};  // discard three of a kind and another pair
            u_int8_t suits[4] = {0};
            // should discard TWO_PAIRy, THREE_OF_A_KINDy, STRAIGHT, FLUSH, 
            // FULL_HOUSEy (discarded by THREE_OF_A_KIND), FOUR_OF_A_KINDy (discarded by THREE_OF_A_KIND), STRAIGHT_FLUSHy (discarded by flush), ROYAL_FLUSHy (discarded by flush)
            std::random_shuffle(cards.begin(), cards.end());
            cards.pop_back();
            cards.pop_back();
            for(u_int8_t j = 0; j < 3; j++){
                cards.push_back(Deck::getRandomCardExcept(cards, -1, ranks));
                ranks.push_back(cards.back().rank);
            }
            for(u_int8_t j = 0; j < cards.size(); j++)
                suits[cards[j].suit]++;

            // add the other cards while avoiding flush and straight
            while(cards.size() < 7){
                cards.push_back(Deck::getRandomCardExcept(cards, -1, ranks));
                suits[cards.back().suit]++;

                // avoid flush
                if(std::find(suits, suits + 4, 5) != suits + 4){
                    suits[cards.back().suit]--;
                    cards.pop_back();
                    continue;
                }
                ranks.push_back(cards.back().rank);
                std::vector<u_int8_t> sortedRanks = ranks;
                std::sort(sortedRanks.begin(), sortedRanks.end());
                // we dont need to delete duplicates because there is only one pair and the rank is only in the vector once
                // avoid straight
                for(u_int8_t j = 0; j < sortedRanks.size() - 4; j++){
                    if(sortedRanks[j] == sortedRanks[j + 4] - 4){
                        suits[cards.back().suit]--;
                        ranks.pop_back();
                        cards.pop_back();
                        break;
                    }
                }
            }
            ranks.erase(ranks.begin()); // remove pair rank
            std::sort(ranks.begin(), ranks.end()); // sort ranks

            u_int32_t rankStrength = rank*2*2*2*2*2*2*2*2*2*2*2*2 + ranks[4]*2*2*2*2*2*2*2*2 + ranks[3]*2*2*2*2 + ranks[2];
            for(u_int8_t iter = 0; iter < 20; iter++){
                EXPECT_EQ(cards.size(), 7);
                std::random_shuffle(cards.begin(), cards.end());
                HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                EXPECT_EQ(hs.handkind, HandKinds::PAIR);
                EXPECT_EQ(hs.rankStrength, rankStrength);
            }
        }
    }
}

TEST(HandStrengths, HighCard) {
    for(u_int64_t iters = 0; iters < ITERATIONS*14; iters++){
        std::vector<Card> cards{};
        std::vector<u_int8_t> ranks{};
        cards.reserve(7);
        ranks.reserve(7);
        u_int8_t suits[4] = {0};
        u_int32_t rankStrength = 0;
        // should discard PAIR, STRAIGHT, FLUSH,
        // TWO_PAIRy (discarded by PAIR), THREE_OF_A_KINDy (discarded by PAIR),
        // FULL_HOUSEy (discarded by PAIR), FOUR_OF_A_KINDy (discarded by PAIR), STRAIGHT_FLUSHy (discarded by flush), ROYAL_FLUSHy (discarded by flush)
        
        // add the other cards while avoiding flush, straight and pair
        while(cards.size() < 7){
            cards.push_back(Deck::getRandomCardExcept(cards, -1, ranks));
            suits[cards.back().suit]++;

            // avoid flush
            if(std::find(suits, suits + 4, 5) != suits + 4){
                suits[cards.back().suit]--;
                cards.pop_back();
                continue;
            }
            ranks.push_back(cards.back().rank);
            std::vector<u_int8_t> sortedRanks = ranks;
            std::sort(sortedRanks.begin(), sortedRanks.end());
            // we dont need to delete duplicates because there is no pair
            // avoid straight
            for(int8_t j = 0; j < sortedRanks.size() - 4; j++){
                if(sortedRanks[j] == sortedRanks[j + 4] - 4){
                    suits[cards.back().suit]--;
                    ranks.pop_back();
                    cards.pop_back();
                    break;
                }
            }
        }

        std::sort(ranks.begin(), ranks.end()); // sort ranks

        rankStrength = ranks[6]*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2 + ranks[5]*2*2*2*2*2*2*2*2*2*2*2*2 + ranks[4]*2*2*2*2*2*2*2*2 + ranks[3]*2*2*2*2 + ranks[2];
        for(u_int8_t iter = 0; iter < 20; iter++){
            EXPECT_EQ(cards.size(), 7);
            std::random_shuffle(cards.begin(), cards.end());
            HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
            EXPECT_EQ(hs.handkind, HandKinds::HIGH_CARD);
            EXPECT_EQ(hs.rankStrength, rankStrength);
        }
    }
}
