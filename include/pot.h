#pragma once

#include "config.h"

class Pot {
   public:
    Pot() noexcept { this->reset(); };

    // light method allows reuse of the pot object by being performant
    void reset() noexcept;

    // isCapped() returns true if the pot is capped, false otherwise
    bool isCapped() const noexcept { return this->potCap != 0; };

    // sets the pot cap
    void setCap(const u_int64_t cap) noexcept { this->potCap = cap; };

    // adds chips to the pot up to the cap if there is one
    // previousBet is the bet that was made before the current one, wholeBet is the bet that is being made (with the previousBet)
    // the chips that could not be added to the pot are returned
    u_int64_t addChips(const u_int64_t wholeBet, const u_int64_t previousBet = 0) noexcept;

    // returns the pot value
    u_int64_t getPotValue() const noexcept { return this->potValue; };

    // adds a player to the non-contenders array
    void addNonContender(const u_int8_t playerPos) noexcept { this->nonContenders[playerPos] = true; };

    // returns true if the player is a contender, false otherwise
    bool isContender(const u_int8_t playerPos) const noexcept { return !this->nonContenders[playerPos]; };

   private:
    u_int64_t potValue;  // Pot value
    u_int64_t potCap;    // Pot cap (0 if there is none), used to cap the bet size for each player
    // Array of non-contenders (players who are not fighting for the pot) FOLDS AND OUTS ARE NOT CONSIDERED NON-CONTENDERS
    // used for side pots where the all in player(s) are not fighting for the pot
    // true if the player does not fight for the pot, false otherwise
    bool nonContenders[MAX_PLAYERS];
};