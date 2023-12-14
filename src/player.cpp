#include "player.h"

const std::string Player::getName() const noexcept{
    return std::to_string(this->playerPosNum) + ":" + this->name;
}

void Player::setPlayerPosNum(u_int8_t num) noexcept {
    this->playerPosNum = num;
}

void Player::setHand(const Card card1, const Card card2) noexcept{
    this->hand = std::pair<Card, Card>(card1, card2);
}
