#pragma once
#include <iostream>

#include "data_structs.h"
#include "deck.h"

class Player {
   public:
    Player() noexcept = default;

    virtual const char* getName() const noexcept final;
    virtual void setPlayerPosNum(u_int8_t num) noexcept final;
    virtual void gameWon() noexcept final { wins++; };
    virtual u_int32_t getWins() const noexcept final { return wins; };

    virtual void setHand(const Card card1, const Card card2) noexcept final;
    virtual const std::pair<Card, Card> getHand() const noexcept final;

    virtual Action turn(const Data& data, const bool onlyRaise = false) const noexcept = 0;

    virtual ~Player() noexcept = default;

   protected:
    Player(const char* name) {
        if (std::strlen(name) > MAX_PLAYER_NAME_LENGTH) {
            PLOG_FATAL << "Player name too long";
            throw std::invalid_argument("Player name too long");
        }
        std::strncpy(this->name, name, MAX_PLAYER_NAME_LENGTH);
    };
    char name[MAX_PLAYER_NAME_LENGTH];

   private:
    std::pair<Card, Card> hand;
    u_int8_t playerPosNum = 0;
    u_int32_t wins = 0;
};