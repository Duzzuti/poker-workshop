#include "player.h"

const char* Player::getName() const noexcept {
    static char s[MAX_PLAYER_GET_NAME_LENGTH];
    // format: "playerPosNum:name"
    std::snprintf(s, sizeof(s), "%d:%s", this->playerPosNum, this->name);
    return s;
}

Player::Player(const char* name) {
    if (std::strlen(name) > MAX_PLAYER_NAME_LENGTH) {
        PLOG_FATAL << "Player name too long";
        throw std::invalid_argument("Player name too long");
    }
    // copy name to this->name
    std::strncpy(this->name, name, MAX_PLAYER_NAME_LENGTH);
}

const char* Player::createPlayerName(const char* name, const u_int8_t playerNum, const bool appendNum) noexcept {
    if (appendNum) return name;

    static char playerName[MAX_PLAYER_NAME_LENGTH];
    // format: "nameNum"
    std::snprintf(playerName, sizeof(playerName), "%s%d", name, playerNum);
    return playerName;
};
