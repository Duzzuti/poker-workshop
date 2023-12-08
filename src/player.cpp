#include "player.h"

void Player::setChips(u_int64_t chips) noexcept {
    this->chips = chips;
}

u_int64_t Player::getChips() const noexcept {
    return this->chips;
}

const std::string &Player::getName() const noexcept{
    return this->name;
}

void Player::addChips(u_int64_t chips) noexcept {
    this->chips += chips;
}

bool Player::removeChips(u_int64_t chips) noexcept {
    if(this->chips < chips)
        return false;
    this->chips -= chips;
    return true;
}

bool Player::hasChips() const noexcept {
    return this->chips > 0;
}

void Player::setHand(const Card card1, const Card card2) noexcept{
    this->hand = std::pair<Card, Card>(card1, card2);
}
