#pragma once
#include <iostream>

#include "deck.h"
#include "data_structs.h"

class Player {
public:
    Player() noexcept = default;

    virtual const std::string getName() const noexcept final;
    virtual void setPlayerPosNum(u_int8_t num) noexcept final;

    virtual void setHand(const Card card1, const Card card2) noexcept final;
    virtual const std::pair<Card, Card> getHand() const noexcept final;
    
    virtual Action turn(const Data& data) const noexcept = 0;

    virtual ~Player() noexcept = default;

protected:
    Player(const std::string& name) noexcept : name(name) {};
    const std::string name;


private:
    std::pair<Card, Card> hand;
    u_int8_t playerPosNum;
};