#pragma once

#include "config.h"

/// @brief Pot class contains functionalities for a pot in the game
/// @note There are multiple pots in the game (main pot, side pots)
class Pot {
   public:
    /// @brief Default constructor
    /// @exception Guarantee No-throw
    constexpr Pot() noexcept { this->reset(); };

    /// @brief Reset method allows reuse of the pot object by being performant
    /// @exception Guarantee No-throw
    /// @note Resets all values to their defaults
    constexpr void reset() noexcept {
        this->potValue = 0;
        this->potCap = 0;
        std::memset(this->nonContenders, 0, sizeof(this->nonContenders));
    };

    /// @brief Checks if the pot is capped
    /// @return True if the pot is capped, false otherwise
    /// @exception Guarantee No-throw
    constexpr bool isCapped() const noexcept { return this->potCap != 0; };

    /// @brief Sets the cap of the pot
    /// @param cap The cap of the pot
    /// @exception Guarantee No-throw
    constexpr void setCap(const u_int64_t cap) noexcept { this->potCap = cap; };

    /// @brief Adds chips to the pot up to the cap if there is one
    /// @param previousBet The bet that was made before the current one (from the same player)
    /// @param wholeBet is the bet that is being made (with the previousBet)
    /// @return The chips that could not be added to the pot (because of the cap)
    /// @exception Guarantee No-throw
    /// @note If the pot is capped, the chips are added up to the cap and the rest is returned
    /// @note If the pot is not capped, the chips are added to the pot and 0 is returned
    constexpr u_int64_t addChips(const u_int64_t wholeBet, const u_int64_t previousBet = 0) noexcept {
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

    /// @brief Gets the pot value
    /// @return The pot value
    /// @exception Guarantee No-throw
    constexpr u_int64_t getPotValue() const noexcept { return this->potValue; };

    /// @brief Adds a player to the non-contenders array
    /// @param playerPos The playerPos of the player to be added
    /// @exception Guarantee No-throw
    constexpr void addNonContender(const u_int8_t playerPos) noexcept { this->nonContenders[playerPos] = true; };

    /// @brief Checks if the player is a contender
    /// @param playerPos The playerPos of the player to be checked
    /// @return True if the player is a contender, false otherwise
    /// @exception Guarantee No-throw
    constexpr bool isContender(const u_int8_t playerPos) const noexcept { return !this->nonContenders[playerPos]; };

   private:
    /// @brief The amount of chips in the pot
    u_int64_t potValue{};
    /// @brief Pot cap (0 if there is none), used to cap the bet size for each player
    /// @see addChips()
    u_int64_t potCap{};
    /// @brief Array of non-contenders (players who are not fighting for the pot)
    /// @note FOLDS AND OUTS ARE NOT CONSIDERED NON-CONTENDERS
    /// @note Used for side pots where the all in player(s) are not fighting for the pot
    /// @example nonContenders[i] true if the player on position i does not fight for the pot, false otherwise
    bool nonContenders[MAX_PLAYERS]{};
};