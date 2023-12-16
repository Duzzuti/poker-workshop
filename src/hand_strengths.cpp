#include <algorithm>

#include "hand_strengths.h"

std::vector<HandStrengths> HandStrengths::getHandStrengths(Player* players[], const Data& data) noexcept {
    std::vector<HandStrengths> handStrengths;
    handStrengths.reserve(data.numPlayers);

    for(u_int8_t i = 0; i < data.numPlayers; i++){
        if(data.gameData.playerOut[i] || data.roundData.playerFolded[i]){
            handStrengths.emplace_back(HandKinds::NO_HAND, 0);
            continue;
        }
        u_int8_t suits[4] = {0, 0, 0, 0};
        u_int8_t ranks[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0 ,0};
        suits[players[i]->getHand().first.suit]++;
        suits[players[i]->getHand().second.suit]++;
        for(u_int8_t j = 0; j < 5; j++){
            suits[data.roundData.communityCards[j].suit]++;
        }
        ranks[players[i]->getHand().first.rank - 2]++;
        ranks[players[i]->getHand().second.rank - 2]++;
        for(u_int8_t j = 0; j < 5; j++){
            ranks[data.roundData.communityCards[j].rank - 2]++;
        }

        u_int8_t rank11 = 0;
        u_int8_t rank12 = 0;
        u_int8_t rank13 = 0;
        u_int8_t rank14 = 0;
        u_int8_t rank15 = 0;
        u_int8_t rank21 = 0;
        u_int8_t rank22 = 0;
        u_int8_t rank23 = 0;
        u_int8_t rank31 = 0;
        u_int8_t rank32 = 0;
        u_int8_t rank4 = 0;
        int8_t flush_suit = -1;

        for(u_int8_t j = 0; j < 4; j++){
            if(suits[j] >= 5){
                // flush
                flush_suit = j;
                break;
            }
        }

        for(int8_t j = 12; j >= 0; j--){
            switch(ranks[j]){
                case 4:
                    rank4 = j + 2;
                    break;
                case 3:
                    if(rank31 == 0)
                        rank31 = j + 2;
                    else if(rank32 == 0)
                        rank32 = j + 2;
                    break;
                case 2:
                    if(rank21 == 0)
                        rank21 = j + 2;
                    else if(rank22 == 0)
                        rank22 = j + 2;
                    else if(rank23 == 0)
                        rank23 = j + 2;
                    break;
                case 1:
                    if(rank11 == 0)
                        rank11 = j + 2;
                    else if(rank12 == 0)
                        rank12 = j + 2;
                    else if(rank13 == 0)
                        rank13 = j + 2;
                    else if(rank14 == 0)
                        rank14 = j + 2;
                    else if(rank15 == 0)
                        rank15 = j + 2;
                    break;
                default:
                    break;
            }
        }

        // available hands: high card, pair, two pair, three of a kind, straight, flush, full house, four of a kind, straight flush, royal flush

        if(flush_suit != -1){
            // flush 
            // available hands: flush, full house, four of a kind, straight flush, royal flush
            std::vector<Card> flush_cards;
            flush_cards.reserve(7);
            if(players[i]->getHand().first.suit == flush_suit){
                flush_cards.push_back(players[i]->getHand().first);
            }
            if(players[i]->getHand().second.suit == flush_suit){
                flush_cards.push_back(players[i]->getHand().second);
            }
            for(u_int8_t j = 0; j < 5; j++){
                if(data.roundData.communityCards[j].suit == flush_suit){
                    flush_cards.push_back(data.roundData.communityCards[j]);
                }
            }
            std::sort(flush_cards.begin(), flush_cards.end(), [](const Card& card1, const Card& card2){
                return card1.rank < card2.rank;
            });
            u_int8_t flush_length = flush_cards.size();
            for(u_int8_t j = 0; j < flush_length - 4; j++){
                if(flush_cards[j].rank == flush_cards[j + 4].rank + 4){
                    if(flush_cards[j].rank == 14){
                        // royal flush
                        handStrengths.emplace_back(HandKinds::ROYAL_FLUSH, 0);
                    }else{
                        // straight flush
                        handStrengths.emplace_back(HandKinds::STRAIGHT_FLUSH, flush_cards[j].rank);
                    }
                    break;
                }
            }

            if(handStrengths.size() == i){
                if(rank4 != 0){
                    // four of a kind
                    handStrengths.emplace_back(HandKinds::FOUR_OF_A_KIND, (rank4 << 4) | std::max<u_int8_t>(std::max<u_int8_t>(rank11, rank21), rank31));
                }else if(rank31 != 0 && (rank32 != 0 || rank21 != 0)){
                    // full house
                    handStrengths.emplace_back(HandKinds::FULL_HOUSE, (rank31 << 4) | std::max<u_int8_t>(rank32, rank21));
                }else{
                    // flush
                    handStrengths.emplace_back(HandKinds::FLUSH, getRankStrength(flush_cards, 5));
                }
            }
            continue;
        }else{
            // no flush
            // available hands: high card, pair, two pair, three of a kind, straight, full house, four of a kind
            if(rank4 != 0){
                // four of a kind
                handStrengths.emplace_back(HandKinds::FOUR_OF_A_KIND, (rank4 << 4) | std::max<u_int8_t>(std::max<u_int8_t>(rank11, rank21), rank31));
                continue;
            }
            if(rank31 != 0 && (rank32 != 0 || rank21 != 0)){
                // full house
                handStrengths.emplace_back(HandKinds::FULL_HOUSE, (rank31 << 4) | std::max<u_int8_t>(rank32, rank21));
                continue;
            }
            // available hands: high card, pair, two pair, three of a kind, straight
            for(int8_t j = 12; j >= 4; j--){
                if(ranks[j] != 0 && ranks[j - 1] != 0 && ranks[j - 2] != 0 && ranks[j - 3] != 0 && ranks[j - 4] != 0){
                    // straight
                    handStrengths.emplace_back(HandKinds::STRAIGHT, j + 2);
                    break;
                }
            }
            if(handStrengths.size() == i){
                if(rank31 != 0){
                    // three of a kind
                    handStrengths.emplace_back(HandKinds::THREE_OF_A_KIND, (rank31 << 8) | (rank11 << 4) | rank12);
                }else if(rank21 != 0 && rank22 != 0){
                    // two pair
                    handStrengths.emplace_back(HandKinds::TWO_PAIR, (rank21 << 8) | (rank22 << 4) | std::max<u_int8_t>(rank11, rank23));
                }else if(rank21 != 0){
                    // pair
                    handStrengths.emplace_back(HandKinds::PAIR, (rank21 << 12) | (rank11 << 8) | (rank12 << 4) | rank13);
                }else{
                    // high card
                    handStrengths.emplace_back(HandKinds::HIGH_CARD, (rank11 << 16) | (rank12 << 12) | (rank13 << 8) | (rank14 << 4) | rank15);
                }
            }
        }
    }
    return handStrengths;
}

u_int32_t HandStrengths::getRankStrength(const std::vector<Card> sortedCards, const u_int8_t num) noexcept {
    u_int32_t rankStrength = 0;
    for(u_int8_t i = 0; i < num; i++){
        rankStrength |= sortedCards[i].rank << 4*(num-i-1);
    }
    return rankStrength;
}
