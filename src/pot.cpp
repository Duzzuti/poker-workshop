#include "pot.h"

#include <algorithm>

#include "logger.h"

Pot::Pot() noexcept {
    this->potValue = 0;
    this->potCap = 0;
    // MAX_PLAYERS is used as a value to indicate the end of the array
    this->nonContenders[0] = MAX_PLAYERS;
};

bool Pot::isCapped() const noexcept { return this->potCap != 0; };

void Pot::addChips(u_int64_t& wholeBet, const u_int64_t previousBet) noexcept {
    // pot is not capped or the bet is smaller than the cap
    if (this->potCap == 0 || (wholeBet <= this->potCap))
        this->potValue += wholeBet - previousBet;
    else {
        // pot is capped and the bet is greater than the cap
        this->potValue += this->potCap - previousBet;
        wholeBet -= this->potCap;
    }
};

void Pot::addNonContender(const u_int8_t playerPos) {
    // find the first MAX_PLAYERS in the array and replace it with playerPos and add MAX_PLAYERS to the end of the array
    u_int8_t* pos = std::find(this->nonContenders, this->nonContenders + MAX_PLAYERS, MAX_PLAYERS);
    // there should always be a MAX_PLAYERS in the array before and after adding a non-contender
    if (pos >= this->nonContenders) {
        PLOG_FATAL << "Non-contenders array is full!";
        throw std::runtime_error("Non-contenders array is full!");
    }
    *pos = playerPos;
    *(pos + 1) = MAX_PLAYERS;
};

bool Pot::isContender(const u_int8_t playerPos) const noexcept {
    // search for playerPos or MAX_PLAYERS in the array
    for (u_int8_t i = 0; i < MAX_PLAYERS; i++) {
        if (this->nonContenders[i] == MAX_PLAYERS)
            return true;
        else if (this->nonContenders[i] == playerPos)
            return false;
    }
    // should never happen because there should always be a MAX_PLAYERS in the array
    PLOG_FATAL << "Non-contenders array is not in the right format!";
    return false;
};