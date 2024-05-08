#include "hand_utils.h"

#include <fstream>

constexpr u_int8_t HandUtils::getHandIndex(const std::pair<Card, Card> playerCards) noexcept {
    const int16_t r1 = std::max(playerCards.first.rank, playerCards.second.rank);
    const int16_t r2 = std::min(playerCards.first.rank, playerCards.second.rank);
    return r1 + r2 - 4 + (r1 - 2) * (r1 - 3) / 2;
}

constexpr unsigned char* HandUtils::getHandName(int8_t handIndex) noexcept {
    // remove the possible ranks until the index is negative or 0. r1 is now the last removed rank + 1
    // r2 is r1 if the index is 0, otherwise the index + rank
    u_int8_t r1 = 0;
    u_int8_t r2 = 0;
    for (u_int8_t i = 2; i < 14; i++) {
        handIndex -= i;
        if (handIndex <= 0) {
            r1 = i + 1;
            r2 = handIndex == 0 ? i + 1 : handIndex + i;
            break;
        }
    }
    // fill the name array with the ranks and the null terminator
    HandUtils::name[0] = HandUtils::ranks[r1 - 2];
    HandUtils::name[1] = HandUtils::ranks[r2 - 2];
    HandUtils::name[2] = '\0';
    return HandUtils::name;
}

void HandUtils::evaluateHands(const Card communityCards[], const std::pair<Card, Card> playerCards[], const u_int8_t players) noexcept {
    // set player 0 as the default winner and their hand as the strongest
    u_int8_t winners[players] = {0};
    u_int8_t numWinners = 1;
    HandStrengths strongestHand = HandStrengths::getHandStrength(playerCards[0], communityCards);
    HandStrengths tmpHand;
    // compare the hands of all players to find the strongest
    for (u_int8_t p = 1; p < players; p++) {
        tmpHand = HandStrengths::getHandStrength(playerCards[p], communityCards);
        if (tmpHand > strongestHand) {
            // new strongest hand, new winner
            strongestHand = tmpHand;
            numWinners = 1;
            winners[0] = p;
        } else if (tmpHand == strongestHand)
            // same strength, add to winners
            winners[numWinners++] = p;
    }
    // update the internal arrays with the winners and the total count for each hand
    this->addWinners(playerCards, winners, numWinners, players);
}

void HandUtils::writeResults(const std::string& filename, const u_int8_t players, const bool newFile) const noexcept {
    // write hand + total + hand/total in csv file
    std::ofstream file(filename, newFile ? std::ios::trunc : std::ios::app);
    if (newFile) file << "Players, Hand, Suited, Wins, Total, Wins/Total, Wins/Total*Players\n";
    for (u_int8_t i = 0; i < 91; i++) {
        // TODO: add hand names
        file << +players << ", " << +i << ", true, " << this->handsSuited[i] << ", " << this->handsSuitedTotal[i] << ", " << (double)this->handsSuited[i] / this->handsSuitedTotal[i] << ", "
             << (double)this->handsSuited[i] / this->handsSuitedTotal[i] * players << ",\n";
        file << +players << ", " << +i << ", false, " << this->handsUnsuited[i] << ", " << this->handsUnsuitedTotal[i] << ", " << (double)this->handsUnsuited[i] / this->handsUnsuitedTotal[i] << ", "
             << (double)this->handsUnsuited[i] / this->handsUnsuitedTotal[i] * players << ",\n";
    }
    file.close();
}

void HandUtils::addWinners(const std::pair<Card, Card> playerCards[], const u_int8_t winners[], const u_int8_t numWinners, const u_int8_t players) noexcept {
    // the amount that is added to the win stat if the hand wins
    const u_int8_t add = (numWinners == 1 ? this->winnerAdd : this->splitAdd);
    // iterate over all player hands
    for (u_int8_t i = 0; i < players; i++) {
        const u_int8_t ind = this->getHandIndex(playerCards[i]);
        // add total to the total count and only if the hand wins, add to the win count
        if (playerCards[i].first.suit == playerCards[i].second.suit) {
            // suited
            this->handsSuitedTotal[ind] += this->totalAdd;
            if (std::find(winners, winners + numWinners, i) != winners + numWinners) this->handsSuited[ind] += add;
        } else {
            // offsuited
            this->handsUnsuitedTotal[ind] += this->totalAdd;
            if (std::find(winners, winners + numWinners, i) != winners + numWinners) this->handsUnsuited[ind] += add;
        }
    }
}
