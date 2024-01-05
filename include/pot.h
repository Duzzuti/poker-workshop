#pragma once

#include "config.h"

class Pot {
   public:
    Pot() noexcept;

    // light method allows reuse of the pot object by being performant
    void reset() noexcept {
        this->potValue = 0;
        this->potCap = 0;
        this->nonContenders[0] = MAX_PLAYERS;
    };

    // isCapped() returns true if the pot is capped, false otherwise
    bool isCapped() const noexcept;

    // sets the pot cap
    void setCap(const u_int64_t cap) noexcept { this->potCap = cap; };

    // adds chips to the pot up to the cap if there is one
    // previousBet is the bet that was made before the current one, wholeBet is the bet that is being made (with the previousBet)
    // if the pot is capped, wholeBet is reduced by the amount of chips that were added to the pot
    void addChips(u_int64_t& wholeBet, const u_int64_t previousBet = 0) noexcept;

    // returns the pot value
    u_int64_t getPotValue() const noexcept { return this->potValue; };

    // adds a player to the non-contenders array
    void addNonContender(const u_int8_t playerPos);

    // returns true if the player is a contender, false otherwise
    bool isContender(const u_int8_t playerPos) const noexcept;

   private:
    u_int64_t potValue;  // Pot value
    u_int64_t potCap;    // Pot cap (0 if there is none), used to cap the bet size for each player
    // Array of non-contenders (players who are not fighting for the pot) FOLDS AND OUTS ARE NOT CONSIDERED NON-CONTENDERS
    // used for side pots where the all in player(s) are not fighting for the pot
    // MAX_PLAYERS is used as a value to indicate the end of the array
    // Example: nonContenders = {2, 1, 3, MAX_PLAYERS, ?, ?, ?, ?, ?, ?} means that players 2, 1 and 3 are non-contenders
    u_int8_t nonContenders[MAX_PLAYERS];
};