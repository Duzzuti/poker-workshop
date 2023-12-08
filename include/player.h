#pragma once
#include <iostream>

#include "deck.h"
#include "data_structs.h"

class Player {
public:
    Player() noexcept = default;

    virtual u_int64_t getChips() const noexcept final;
    virtual const std::string& getName() const noexcept final;

    virtual void setChips(u_int64_t chips) noexcept final;
    virtual void setHand(const Card card1, const Card card2) noexcept final;
    
    virtual void addChips(u_int64_t chips) noexcept final;
    virtual bool removeChips(u_int64_t chips) noexcept final;
    
    virtual bool hasChips() const noexcept final;

    virtual Action turn(const TurnData& data) const noexcept = 0;

    virtual ~Player() noexcept = default;

protected:
    Player(const std::string& name) noexcept : name(name) {};
    const std::string name;


private:
    u_int64_t chips;
    std::pair<Card, Card> hand;
};