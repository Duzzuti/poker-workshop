#include <gtest/gtest.h>
#include <algorithm>

#include "hand_strengths.h"

/* 
every test generates random 7 cards that form the tested hand.
the tests are designed to generate every possible hand for every category.
the tests are generating random combination. You can set the ITERATIONS constant to a higher value to generate more combinations.
*/
const constexpr u_int64_t ITERATIONS = 1000;

// returns a reverse sorted vector of ranks of the cards in the range [begin, end)
std::vector<u_int8_t> getSortedRanks(const std::vector<Card>::iterator& begin, const std::vector<Card>::iterator& end) noexcept {
    std::vector<u_int8_t> ranks;
    ranks.reserve(end - begin); // reserve enough space
    // add all ranks to the vector
    for(std::vector<Card>::iterator i = begin; i < end; i++){
        ranks.push_back(i->rank);
    }
    // sort and reverse the vector
    std::sort(ranks.begin(), ranks.end());
    std::reverse(ranks.begin(), ranks.end());
    return ranks;
}

TEST(HandStrengths, RoyalFlush) {
    // generate royal flush hands and test if it is detected as royal flush
    for(u_int64_t iters = 0; iters < ITERATIONS*8; iters++){
        // generate royal flush for every suit
        for(u_int8_t suit = 0; suit < 4; suit++){
            // royal flush hand
            std::vector<Card> cards{Card{14, suit}, Card{13, suit}, Card{12, suit}, Card{11, suit}, Card{10, suit}};
            // add two random cards
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
    // generate straight flush hands and test if it is detected as straight flush
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
        // generate straight flush for every suit
        for(u_int8_t suit = 0; suit < 4; suit++){
            // generate every possible straight in the suit (without royal flush, where rank = 14)
            for(u_int8_t rank = 6; rank < 14; rank++){
                std::vector<Card> cards{Card{rank, suit}, Card{(u_int8_t)(rank - 1), suit}, Card{(u_int8_t)(rank - 2), suit}, Card{(u_int8_t)(rank - 3), suit}, Card{(u_int8_t)(rank - 4), suit}};
                // avoid that the straight gets higher than rank
                std::vector<Card> cards2 = cards;
                cards2.push_back(Card{(u_int8_t)(rank + 1), suit});
                cards.push_back(Deck::getRandomCardExcept(cards2));
                cards2.push_back(cards.back());
                cards.push_back(Deck::getRandomCardExcept(cards2));
                for(u_int8_t iter = 0; iter < 20; iter++){
                    EXPECT_EQ(cards.size(), 7);
                    std::random_shuffle(cards.begin(), cards.end());
                    HandStrengths hs = HandStrengths::getHandStrength(std::pair<Card, Card>{cards[0], cards[1]}, std::vector<Card>{cards[2], cards[3], cards[4], cards[5], cards[6]});
                    EXPECT_EQ(hs.handkind, HandKinds::STRAIGHT_FLUSH);
                    EXPECT_EQ(hs.rankStrength, +rank);  // rank strength is the highest rank of the straight
                }
            }
        }
    }
}

TEST(HandStrengths, FourOfAKind) {
    // generate four of a kind hands and test if it is detected as four of a kind
    for(u_int64_t iters = 0; iters < ITERATIONS*2; iters++){
        // generate four of a kind for every rank
        for(u_int8_t rank = 2; rank < 15; rank++){
            // there cannot be a straight flush because there are only 3 extra cards
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            cards.push_back(Deck::getRandomCardExcept(cards));
            cards.push_back(Deck::getRandomCardExcept(cards));
            cards.push_back(Deck::getRandomCardExcept(cards));
            // rank strength is the rank of the four of a kind + the highest rank of the extra cards
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
    // generate full house hands and test if it is detected as full house
    for(u_int64_t iters = 0; iters < ITERATIONS; iters++){
        // generate the triple of full house for every rank
        for(u_int8_t rank = 2; rank < 15; rank++){
            // start with a quad and remove a random card
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            u_int8_t ranks[15] = {0};
            // should discard FOUR_OF_A_KINDy (flush is not possible)
            std::random_shuffle(cards.begin(), cards.end());
            cards.pop_back();   // select one triple
            // add three random cards by avoiding quads
            cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank}));
            ranks[cards.back().rank]++;
            cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank}));
            ranks[cards.back().rank]++;
            cards.push_back(Deck::getRandomCardExcept(cards, -1, {rank}));
            ranks[cards.back().rank]++;

            // make sure that there is at least one other pair by also avoiding another quad
            Card card = Deck::getRandomCardExcept(cards, -1, {rank});
            ranks[card.rank]++;
            while(std::count(ranks, ranks + 15, 2) == 0 && std::count(ranks, ranks + 15, 3) == 0){
                // draw new card if there is no pair or triple found besides the rank triple
                ranks[card.rank]--;
                card = Deck::getRandomCardExcept(cards, -1, {rank});
                ranks[card.rank]++;
            }
            cards.push_back(card);

            // calculate rankStrength
            u_int32_t rankStrength = 0;
            // iterate over the ranks from highest to lowest
            // save the triple rank into 0x000000F0 of rankStrength and the pair rank into 0x0000000F of rankStrength
            for(u_int8_t j = 14; j >= 2; j--){
                if(j == rank){
                    // if the looped rank is below the triple rank and we did not found a triple yet, add the rank to the triple rank
                    // if the looped rank is below the triple rank and we did found a triple yet, add the rank to the pair rank
                    if((rankStrength & 0x000000F0) == 0)
                        rankStrength += rank*2*2*2*2;
                    else if((rankStrength & 0x0000000F) == 0)
                        rankStrength += rank;
                }else if(ranks[j] == 2 && ((rankStrength & 0x0000000F) == 0))
                    // if the looped rank is a pair and we did not found a pair yet, add the rank to the pair rank
                    rankStrength += j;
                else if(ranks[j] == 3 && ((rankStrength & 0x000000F0) == 0))
                    // if the looped rank is a triple and we did not found a triple yet, add the rank to the triple rank
                    rankStrength += j*2*2*2*2;
                else if(ranks[j] == 3 && ((rankStrength & 0x000000F0) != 0) && ((rankStrength & 0x0000000F) == 0))
                    // if the looped rank is a triple and we did found a triple yet but no pair, add the rank to the pair rank
                    rankStrength += j;
                else if(((rankStrength & 0x000000F0) != 0) && ((rankStrength & 0x0000000F) != 0))
                    // if we found a triple and a pair, break
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
    // generate flush hands and test if it is detected as flush
    for(u_int64_t iters = 0; iters < ITERATIONS/30 + 1; iters++){
        // generate flush for every suit
        for(u_int8_t suit = 0; suit < 4; suit++){
            // store every choosen card and every card with the choosen suit
            std::vector<Card> cards{};
            std::vector<Card> flushCards{};
            cards.reserve(7);
            flushCards.reserve(7);
            // should discard FULL_HOUSEy (impossible), FOUR_OF_A_KINDy (impossible), STRAIGHT_FLUSHy, ROYAL_FLUSHy
            // we only need to discard straight flushes
            while(true){
                // clear cards and flushCards
                cards.clear();
                flushCards.clear();
                // add 7 random cards
                for(u_int8_t j = 0; j < 7; j++){
                    cards.push_back(Deck::getRandomCardExcept(cards));
                    if(cards.back().suit == suit)
                        flushCards.push_back(cards.back());
                }
                // check if there is a flush
                if(flushCards.size() < 5)
                    continue;
                // sort the flush cards, there are no duplicates because there is only one card of each rank per suit
                std::sort(flushCards.begin(), flushCards.end(), [](const Card& a, const Card& b) -> bool {return a.rank > b.rank;});
                bool breakFlag = true;  // break if there is no straight
                for(u_int8_t j = 0; j < flushCards.size() - 4; j++){
                    if(flushCards[j].rank == flushCards[j + 4].rank + 4){
                        // there is a straight, continue
                        breakFlag = false;
                        break;
                    }
                }
                if(breakFlag)
                    break;
            }
            
            // flushcards are sorted from highest to lowest rank
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
    // generate straight hands and test if it is detected as straight
    for(u_int64_t iters = 0; iters < ITERATIONS*4; iters++){
        // generate straights for every rank (rank is the highest rank of the straight)
        for(u_int8_t rank = 6; rank < 15; rank++){
            std::vector<Card> cards{};
            cards.reserve(7);
            // should discard FLUSHy, FULL_HOUSEy (impossible), FOUR_OF_A_KINDy (impossible), STRAIGHT_FLUSHy, ROYAL_FLUSHy
            // we only need to discard flushes
            // select random straight
            u_int8_t suits[4] = {0};
            for(u_int8_t j = rank - 4; j <= rank; j++){
                // add random card of rank j
                std::vector<Card> cards1{Card{j, 0}, Card{j, 1}, Card{j, 2}, Card{j, 3}};
                std::random_shuffle(cards1.begin(), cards1.end());
                suits[cards1.back().suit]++;
                // add card if it does not create a flush
                if(std::count(suits, suits + 4, 5) == 0)
                    cards.push_back(cards1.back());
                else{
                    suits[cards1.back().suit]--;
                    j--;    // try again with the same rank
                }
            }
            // add two random cards
            for(u_int8_t j = 0; j < 2; j++){
                while(true){
                    cards.push_back(Deck::getRandomCardExcept(cards, -1, {(u_int8_t)(rank + 1)}));  // dont spawn higher straights
                    suits[cards.back().suit]++;
                    // check if there is a flush
                    if(std::count(suits, suits + 4, 5) == 0)
                        break;
                    else{
                        suits[cards.back().suit]--;
                        cards.pop_back();
                    }
                }
            }
            // rank strength is the highest rank of the straight
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
    // generate three of a kind hands and test if it is detected as three of a kind
    for(u_int64_t iters = 0; iters < ITERATIONS*2; iters++){
        // generate three of a kind for every rank
        for(u_int8_t rank = 2; rank < 15; rank++){
            // start with a quad and remove one random cards
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            // avoid pairs (full house)
            std::vector<u_int8_t> ranks{rank};
            // should discard STRAIGHTy, FLUSHy, FULL_HOUSEy, FOUR_OF_A_KINDy, STRAIGHT_FLUSHy, ROYAL_FLUSHy
            std::random_shuffle(cards.begin(), cards.end());
            Card popedCard = cards.back();
            cards.pop_back();
            // add three random cards by avoiding quads and pairs
            cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            ranks.push_back(cards.back().rank);
            cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            ranks.push_back(cards.back().rank);
            cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            ranks.push_back(cards.back().rank);

            // avoid straight
            std::sort(ranks.begin(), ranks.end());
            if(ranks[0] == ranks[3] - 3){
                // nearly a straight, we need to avoid one rank lower or higher cards
                ranks.push_back(ranks[0] - 1);
                ranks.push_back(ranks[3] + 1);
            }else if(ranks[0] == ranks[3] - 4){
                // nearly a straight, we need to avoid inbetween ranked cards
                if(ranks[0] != ranks[1] - 1)
                    // avoid rank between ranks[0] and ranks[1]
                    ranks.push_back(ranks[0] + 1);
                else if(ranks[1] != ranks[2] - 1)
                    // avoid rank between ranks[1] and ranks[2]
                    ranks.push_back(ranks[1] + 1);
                else
                    // avoid rank between ranks[2] and ranks[3]
                    ranks.push_back(ranks[2] + 1);
            }

            // avoid flush
            if(cards[3].suit == cards[4].suit && cards[4].suit == cards[5].suit && cards[5].suit != popedCard.suit)
                // if we have 3 cards of the same suit and the poped card is not of the same suit 
                // (that means that one of the triplet also has this suit), we need to avoid the same suit
                cards.push_back(Deck::getRandomCardExcept({}, cards[3].suit, ranks));
            else
                cards.push_back(Deck::getRandomCardExcept({}, -1, ranks));
            
            // get all ranks of the cards besides the triplet
            ranks = getSortedRanks(cards.begin() + 3, cards.end());
            // rank strength is the rank of the triplet + the highest ranks of the extra cards
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
    // generate two pair hands and test if it is detected as two pair
    for(u_int64_t iters = 0; iters < ITERATIONS/6 + 1; iters++){
        // generate two pair for every rank
        for(u_int8_t rank1 = 2; rank1 < 15; rank1++){
            for(u_int8_t rank2 = 2; rank2 < 15; rank2++){
                // we need two different pairs
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
                    // check if there is a flush
                    if(std::count(suits, suits + 4, 5) == 1)
                        continue;
                    // check if there is a triple (full house). Only possible if the 3 random cards are the same rank
                    std::vector<u_int8_t> tmpRanks{0, rank1, rank2};
                    if(cards[4].rank == cards[5].rank){
                        if(cards[5].rank == cards[6].rank)
                            // there is a triple
                            continue;
                        // there are 3 pairs
                        tmpRanks = {rank1, rank2, cards[4].rank};
                    }
                    if(cards[5].rank == cards[6].rank || cards[4].rank == cards[6].rank)
                        // there are 3 pairs
                        tmpRanks = {rank1, rank2, cards[6].rank};
                    // sort the pair ranks to get the highest and second highest pair for rankStrength
                    std::sort(tmpRanks.begin(), tmpRanks.end());
                    rankStrength = tmpRanks[2]*2*2*2*2*2*2*2*2 + tmpRanks[1]*2*2*2*2;
                    
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
    // generate pair hands and test if it is detected as pair
    for(u_int64_t iters = 0; iters < ITERATIONS/2*3; iters++){
        for(u_int8_t rank = 2; rank < 15; rank++){
            // start with a quad and remove two random cards
            std::vector<Card> cards{Card{rank, 0}, Card{rank, 1}, Card{rank, 2}, Card{rank, 3}};
            std::vector<u_int8_t> ranks{rank};  // discard three of a kind and another pair
            u_int8_t suits[4] = {0};
            // should discard TWO_PAIRy, THREE_OF_A_KINDy, STRAIGHTy, FLUSHy, 
            // FULL_HOUSEy (discarded by THREE_OF_A_KIND), FOUR_OF_A_KINDy (discarded by THREE_OF_A_KIND), STRAIGHT_FLUSHy (discarded by flush), ROYAL_FLUSHy (discarded by flush)
            std::random_shuffle(cards.begin(), cards.end());
            cards.pop_back();
            cards.pop_back();
            // add 3 random cards by avoiding triplets and pairs
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
    // generate high card hands and test if it is detected as high card
    for(u_int64_t iters = 0; iters < ITERATIONS*14; iters++){
        std::vector<Card> cards{};
        std::vector<u_int8_t> ranks{};
        cards.reserve(7);
        ranks.reserve(7);
        u_int8_t suits[4] = {0};
        u_int32_t rankStrength = 0;
        // should discard PAIRy, STRAIGHTy, FLUSHy,
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
