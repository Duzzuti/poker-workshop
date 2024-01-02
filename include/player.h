#pragma once
#include <iostream>

#include "data_structs.h"
#include "deck.h"

class Player {
   public:
    Player() noexcept = default;

    virtual const std::string getName() const noexcept final;
    virtual void setPlayerPosNum(u_int8_t num) noexcept final;
    virtual void gameWon() noexcept final { wins++; };
    virtual u_int32_t getWins() const noexcept final { return wins; };

    virtual void setHand(const Card card1, const Card card2) noexcept final;
    virtual const std::pair<Card, Card> getHand() const noexcept final;

    virtual Action turn(const Data& data, const bool onlyRaise = false) const noexcept = 0;

    virtual ~Player() noexcept = default;

   protected:
    Player(const std::string& name) : name(name){if(name.length() > MAX_PLAYER_NAME_LENGTH) throw std::invalid_argument("Player name too long");};
    const std::string name;

   private:
    std::pair<Card, Card> hand;
    u_int8_t playerPosNum;
    u_int32_t wins = 0;
};