#pragma once
#include "config.h"
#include "player.h"

class Game {
public:
    Game(const Config& config) noexcept
        : m_config(config) {}

    void run() const;

private:

    // starts a round by shuffling the deck, setting the dealer and the blinds
    OutEnum startRound(Player* players[], Data& data, Deck& deck, const bool firstRound) const noexcept;

    // sets the blinds for the round by betting the small and big blind automatically
    OutEnum setBlinds(Player* players[], Data& data) const noexcept;

    // sets up the data for a bet round (preflop, flop, turn, river)
    void setupBetRound(Data& data) const noexcept;

    // runs a single bet round (preflop, flop, turn, river)
    OutEnum betRound(Player* players[], Data& data) const;

    // runs a single non out player turn
    OutEnum playerTurn(Player* players[], Data& data, short* firstChecker) const;

    // the current player bets amount and the next player is selected
    // note that amount is the total amount that the player bets (e.g. if the player has to call 200 but he already bet 100 => amount is still 200)
    bool bet(Data& data, const u_int64_t amount) const noexcept;

    // selects the current player as out and returns true if only one player is left in this game, selects the next player
    OutEnum playerOut(Player* players[], Data& data) const noexcept;

    // selects the current player as folded and returns true if only one player is left in this round, selects the next player
    OutEnum playerFolded(Player* players[], Data& data) const noexcept;

    OutEnum getOutEnum(Data& data) const noexcept;

    Config m_config;
};
