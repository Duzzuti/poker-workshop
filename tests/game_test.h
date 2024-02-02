#pragma once
#include "check_player/check_player.h"
#include "game.h"
#include "rand_player/rand_player.h"

/// @brief Mocks the Game class for testing
class GameTest : public Game {
   public:
    /// @copydoc Game::Game(const Config&)
    GameTest(const Config& config) noexcept : Game(config) {}

    /// @brief Sets the players of the game
    /// @param players The players of the game should be the same length as Config.numPlayers
    /// @exception Guarantee No-throw
    /// @note The players are moved into the GameTest object
    /// @note Undefined behavior if the length of players is not equal to config.numPlayers
    void setPlayers(std::unique_ptr<Player> players[]) noexcept {
        for (int i = 0; i < this->config.numPlayers; i++) {
            this->players[i] = std::move(players[i]);
        }
    }

    /// @brief Sets the players of the game to a mix of different players that are not for human or testing
    /// @exception Guarantee No-throw
    /// @note Requires Config.numPlayers to be 5, undefined behavior otherwise
    /// @see CheckPlayer
    /// @see RandPlayer
    void setRealisticPlayers() noexcept {
        // set different players
        this->players[0] = std::move(std::make_unique<CheckPlayer>(std::string("a", MAX_PLAYER_NAME_LENGTH).c_str()));
        this->players[1] = std::move(std::make_unique<RandPlayer>(2));
        this->players[2] = std::move(std::make_unique<CheckPlayer>(3));
        this->players[3] = std::move(std::make_unique<RandPlayer>(4));
        this->players[4] = std::move(std::make_unique<RandPlayer>(5));
    }

    /// @copydoc Game::initPlayerOrder()
    void initPlayerOrder() noexcept { Game::initPlayerOrder(); }

    /// @copydoc Game::startRound()
    void startRound(const bool firstRound) { Game::startRound(firstRound); }

    /// @copydoc Game::setBlinds()
    OutEnum setBlinds() noexcept { return Game::setBlinds(); }

    /// @copydoc Game::setupBetRound()
    void setupBetRound() noexcept { Game::setupBetRound(); }

    /// @copydoc Game::betRound()
    OutEnum betRound() { return Game::betRound(); }

    /// @copydoc Game::betRoundContinue()
    bool betRoundContinue(const u_int8_t firstChecker) const noexcept { return Game::betRoundContinue(firstChecker); }

    /// @copydoc Game::bet()
    bool bet(const u_int64_t amount) noexcept { return Game::bet(amount); }

    /// @copydoc Game::playerOut()
    OutEnum playerOut(const char* reason) noexcept { return Game::playerOut(reason); }

    /// @copydoc Game::playerFolded()
    OutEnum playerFolded() noexcept { return Game::playerFolded(); }

    /// @copydoc Game::getOutEnum()
    OutEnum getOutEnum() const noexcept { return Game::getOutEnum(); }

    /// @copydoc Game::preflop()
    void preflop() { Game::preflop(); }

    /// @copydoc Game::flop()
    void flop() { Game::flop(); }

    /// @copydoc Game::turn()
    void turn() { Game::turn(); }

    /// @copydoc Game::river()
    void river() { Game::river(); }

    /// @copydoc Game::getPlayerInfo()
    const char* getPlayerInfo(u_int8_t playerPos = MAX_PLAYERS, const int64_t chipsDiff = 0) const noexcept { return Game::getPlayerInfo(playerPos, chipsDiff); }
};