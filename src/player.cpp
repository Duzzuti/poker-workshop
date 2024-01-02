#include "player.h"

const char* Player::getName() const noexcept {
    static char s[MAX_PLAYER_GET_NAME_LENGTH];
    std::snprintf(s, sizeof(s), "%d:%s", this->playerPosNum, this->name);
    return s;
}

void Player::setPlayerPosNum(u_int8_t num) noexcept { this->playerPosNum = num; }

void Player::setHand(const Card card1, const Card card2) noexcept { this->hand = std::pair<Card, Card>(card1, card2); }

const std::pair<Card, Card> Player::getHand() const noexcept { return this->hand; }
