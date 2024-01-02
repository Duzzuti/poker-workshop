#pragma once
#include "check_player/check_player.h"
#include "game.h"
#include "rand_player/rand_player.h"

class GameTest : public Game {
   public:
    GameTest(const Config& config) noexcept : Game(config) {}

    void setPlayers(std::unique_ptr<Player> players[]) noexcept {
        for (int i = 0; i < this->m_config.numPlayers; i++) {
            this->players[i] = std::move(players[i]);
        }
    }

    void initPlayer() noexcept {
        // init players
        this->players[0] = std::move(std::make_unique<CheckPlayer>(std::string("a", MAX_PLAYER_NAME_LENGTH)));
        this->players[1] = std::move(std::make_unique<RandPlayer>(2));
        this->players[2] = std::move(std::make_unique<CheckPlayer>(3));
        this->players[3] = std::move(std::make_unique<RandPlayer>(4));
        this->players[4] = std::move(std::make_unique<RandPlayer>(5));

        this->data.numPlayers = this->m_config.numPlayers;
    }

    void initPlayerOrder() noexcept { Game::initPlayerOrder(); }

    void startRound(const bool firstRound) { Game::startRound(firstRound); }

    OutEnum setBlinds() noexcept { return Game::setBlinds(); }

    void setupBetRound() noexcept { Game::setupBetRound(); }

    OutEnum betRound() { return Game::betRound(); }

    OutEnum playerTurn(short& firstChecker) { return Game::playerTurn(firstChecker); }

    bool bet(const u_int64_t amount) noexcept { return Game::bet(amount); }

    OutEnum playerOut(const std::string& reason) noexcept { return Game::playerOut(reason); }

    OutEnum playerFolded() noexcept { return Game::playerFolded(); }

    OutEnum getOutEnum() const noexcept { return Game::getOutEnum(); }

    void preflop() { Game::preflop(); }

    void flop() { Game::flop(); }

    void turn() { Game::turn(); }

    void river() { Game::river(); }

    std::string getPlayerInfo(int16_t playerPos = -1, int64_t chipsDiff = 0) const noexcept { return Game::getPlayerInfo(playerPos, chipsDiff); }
};