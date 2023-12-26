#pragma once
#include "game.h"

class GameTest : public Game {
   public:
    GameTest(const Config& config) noexcept : Game(config) {}

    void setPlayers(Player* players[]) noexcept {
        delete[] this->players;
        this->players = players;
    }

    void initPlayerOrder() noexcept { Game::initPlayerOrder(); }

    OutEnum startRound(Deck& deck, const bool firstRound) noexcept { return Game::startRound(deck, firstRound); }

    OutEnum setBlinds() noexcept { return Game::setBlinds(); }

    void setupBetRound() noexcept { Game::setupBetRound(); }

    OutEnum betRound() { return Game::betRound(); }

    OutEnum playerTurn(short* firstChecker) { return Game::playerTurn(firstChecker); }

    bool bet(const u_int64_t amount) noexcept { return Game::bet(amount); }

    OutEnum playerOut() noexcept { return Game::playerOut(); }

    OutEnum playerFolded() noexcept { return Game::playerFolded(); }

    OutEnum getOutEnum() const noexcept { return Game::getOutEnum(); }

    OutEnum preflop(OutEnum lastRes) { return Game::preflop(lastRes); }

    OutEnum flop(Deck& deck, OutEnum lastRes) { return Game::flop(deck, lastRes); }

    OutEnum turn(Deck& deck, OutEnum lastRes) { return Game::turn(deck, lastRes); }

    OutEnum river(Deck& deck, OutEnum lastRes) { return Game::river(deck, lastRes); }

    std::string getPlayerInfo() const noexcept { return Game::getPlayerInfo(); }
};