#pragma once
#include "config.h"
#include "player.h"

class Game {
    friend class GameTest;

   public:
    Game(const Config& config) noexcept : m_config(config), players(new Player*[config.numPlayers]) {}

    void run();

    ~Game() {
        for (int i = 0; i < this->m_config.numPlayers; i++) {
            delete this->players[i];
        }
        delete[] this->players;
    }

   private:
    std::string getPlayerInfo() const noexcept;

    void initPlayerOrder() noexcept;

    // starts a round by shuffling the deck, setting the dealer and the blinds
    OutEnum startRound(const bool firstRound);

    // sets the blinds for the round by betting the small and big blind automatically
    OutEnum setBlinds() noexcept;

    // sets up the data for a bet round (preflop, flop, turn, river)
    void setupBetRound() noexcept;

    // runs a single bet round (preflop, flop, turn, river)
    OutEnum betRound();

    // runs a single non out player turn
    OutEnum playerTurn(short& firstChecker);

    // the current player bets amount and the next player is selected
    // note that amount is the total amount that the player bets (e.g. if the player has to call 200 but he already bet 100 => amount is still 200)
    bool bet(const u_int64_t amount) noexcept;

    // selects the current player as out and returns true if only one player is left in this game, selects the next player
    OutEnum playerOut() noexcept;

    // selects the current player as folded and returns true if only one player is left in this round, selects the next player
    OutEnum playerFolded() noexcept;

    OutEnum getOutEnum() const noexcept;

    void preflop();

    void flop();

    void turn();

    void river();

    Config m_config;
    Player** players;
    Data data;
    Deck deck;
};
