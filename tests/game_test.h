#pragma once
#include "game.h"

class GameTest : public Game{
    public:
        GameTest(const Config& config) noexcept : Game(config) {}

        void setPlayers(Player* players[]) noexcept {
            delete[] this->players;
            this->players = players;
        }

        void initPlayerOrder() noexcept {
            Game::initPlayerOrder();
        }

        OutEnum startRound(Data& data, Deck& deck, const bool firstRound) const noexcept {
            return Game::startRound(data, deck, firstRound);
        }

        OutEnum setBlinds(Data& data) const noexcept {
            return Game::setBlinds(data);
        }

        void setupBetRound(Data& data) const noexcept {
            Game::setupBetRound(data);
        }

        OutEnum betRound(Data& data) const {
            return Game::betRound(data);
        }

        OutEnum playerTurn(Data& data, short* firstChecker) const {
            return Game::playerTurn(data, firstChecker);
        }

        bool bet(Data& data, const u_int64_t amount) const noexcept {
            return Game::bet(data, amount);
        }

        OutEnum playerOut(Data& data) const noexcept {
            return Game::playerOut(data);
        }

        OutEnum playerFolded(Data& data) const noexcept {
            return Game::playerFolded(data);
        }

        OutEnum getOutEnum(const Data& data) const noexcept {
            return Game::getOutEnum(data);
        }

        OutEnum preflop(Data& data, OutEnum lastRes) const noexcept {
            return Game::preflop(data, lastRes);
        }

        OutEnum flop(Data& data, Deck& deck, OutEnum lastRes) const noexcept {
            return Game::flop(data, deck, lastRes);
        }

        OutEnum turn(Data& data, Deck& deck, OutEnum lastRes) const noexcept {
            return Game::turn(data, deck, lastRes);
        }

        OutEnum river(Data& data, Deck& deck, OutEnum lastRes) const noexcept {
            return Game::river(data, deck, lastRes);
        }
};