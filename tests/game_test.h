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

    OutEnum startRound(const bool firstRound) { return Game::startRound(firstRound); }

    OutEnum setBlinds() noexcept { return Game::setBlinds(); }

    void setupBetRound() noexcept { Game::setupBetRound(); }

    OutEnum betRound() { return Game::betRound(); }

    OutEnum playerTurn(short& firstChecker) { return Game::playerTurn(firstChecker); }

    bool bet(const u_int64_t amount) noexcept { return Game::bet(amount); }

    OutEnum playerOut() noexcept { return Game::playerOut(); }

    OutEnum playerFolded() noexcept { return Game::playerFolded(); }

    OutEnum getOutEnum() const noexcept { return Game::getOutEnum(); }

    void preflop() { Game::preflop(); }

    void flop() { Game::flop(); }

    void turn() { Game::turn(); }

    void river() { Game::river(); }

    std::string getPlayerInfo() const noexcept { return Game::getPlayerInfo(); }
};