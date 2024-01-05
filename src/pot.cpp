#include "pot.h"

#include <algorithm>

#include "logger.h"

void Pot::reset() noexcept {
    this->potValue = 0;
    this->potCap = 0;
    std::memset(this->nonContenders, 0, sizeof(this->nonContenders));
};

u_int64_t Pot::addChips(const u_int64_t wholeBet, const u_int64_t previousBet) noexcept {
    // pot is not capped or the bet is smaller than the cap
    if (this->potCap == 0 || (wholeBet <= this->potCap))
        this->potValue += wholeBet - previousBet;
    else {
        // pot is capped and the bet is greater than the cap
        this->potValue += this->potCap - previousBet;
        return wholeBet - this->potCap;
    }
    return 0;
};