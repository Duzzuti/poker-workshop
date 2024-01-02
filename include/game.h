#pragma once
#include <memory>

#include "config.h"
#include "player.h"

class Game {
    friend class GameTest;

   public:
    Game(const Config& config) noexcept : m_config(config), players(new std::unique_ptr<Player>[config.numPlayers]) {}

    void run();

    ~Game() { delete[] this->players; }

   private:
    std::string getPlayerInfo(int16_t playerPos = -1, int64_t chipsDiff = 0) const noexcept;

    void initPlayerOrder() noexcept;

    // starts a round by shuffling the deck, setting the dealer and the blinds
    void startRound(const bool firstRound);

    // sets the blinds for the round by betting the small and big blind automatically
    OutEnum setBlinds() noexcept;

    // sets up the data for a bet round (preflop, flop, turn, river)
    void setupBetRound() noexcept;

    // runs a single bet round (preflop, flop, turn, river)
    OutEnum betRound();

    // returns true if the bet round should continue
    bool betRoundContinue(short firstChecker) const noexcept;

    // true if the current player is active (not out or folded)
    bool currentPlayerActive() const noexcept;

    // true if special condition is met that the current player can only raise or call
    bool currentPlayerCanOnlyRaiseOrCall() const noexcept;

    // runs a single non out player turn
    OutEnum playerTurn(short& firstChecker);

    // runs a single non out player turn where the player can only raise or call
    OutEnum playerTurnOnlyRaise();

    // the current player bets amount and the next player is selected
    // note that amount is the total amount that the player bets (e.g. if the player has to call 200 but he already bet 100 => amount is still 200)
    bool bet(const u_int64_t amount, const bool isBlind = false) noexcept;

    // selects the current player as out and returns true if only one player is left in this game, selects the next player
    OutEnum playerOut(const std::string& reason) noexcept;

    // selects the current player as folded and returns true if only one player is left in this round, selects the next player
    OutEnum playerFolded() noexcept;

    OutEnum getOutEnum() const noexcept;

    void preflop();

    void flop();

    void turn();

    void river();

    Config m_config;
    std::unique_ptr<Player>* players;
    Data data;
    Deck deck;
};
