#include "handstrengths_utils.h"

void HandStrengthsOptions::setFileName(const u_int8_t options[3]) noexcept {
    this->fileName = STR_TOOL_HANDSTRENGTHS_DEFAULT_FILE_NAME + std::to_string(options[0]) + std::to_string(options[1]) + std::to_string(options[2]) + ".csv";
}

void HandStrengthsOptions::setFileName(const u_int8_t winnerAdd, const u_int8_t splitAdd, const u_int8_t totalAdd) noexcept {
    this->fileName = STR_TOOL_HANDSTRENGTHS_DEFAULT_FILE_NAME + std::to_string(winnerAdd) + std::to_string(splitAdd) + std::to_string(totalAdd) + ".csv";
}

void HandStrengthsOptions::setShowOptions(const bool showAttributes[5], const bool showDifferences) noexcept {
    std::memcpy(this->showAttributes, showAttributes, sizeof(this->showAttributes));
    this->showDifferences = showDifferences;
}

void HandStrengthsOptions::setFilterOptions(const bool filterPlayers[MAX_PLAYERS + 1], const bool filterHands[HAND_INDEX_COUNT]) noexcept {
    std::memcpy(this->filterPlayers, filterPlayers, sizeof(this->filterPlayers));
    std::memcpy(this->filterHands, filterHands, sizeof(this->filterHands));
    // set majority settings
    u_int8_t count = 0;
    for (u_int8_t i = 2; i <= MAX_PLAYERS; i++) count += filterPlayers[i];
    this->filterPlayerMajoritySet = count > (MAX_PLAYERS - 1) / 2;
    count = 0;
    for (u_int8_t i = 0; i < HAND_INDEX_COUNT; i++) count += filterHands[i];
    this->filterHandMajoritySet = count > HAND_INDEX_COUNT / 2;
}

bool HandStrengthsOptions::acceptSet(const HandStrengthsSet& set) const noexcept {
    // Set has the format "Players, Hand, Suited, Name, Wins, Total, Wins/Total, Wins/Total*Players"
    const unsigned char playersInt = set.getPlayers();
    const unsigned char handInt = set.getHandIndex();
    if (playersInt > MAX_PLAYERS || handInt >= HAND_INDEX_COUNT || playersInt < 2) {
        PLOG_ERROR << "Invalid set: " << set << " (players: " << +playersInt << ", hand: " << +handInt << ")";
        return false;
    }
    if (this->filterPlayers[playersInt] && this->filterHands[handInt]) {
        // check for invalid hand (r1r2s with r1 == r2)
        const char* name = set.getName();
        return !(name[0] == name[1] && name[2] == 's');
    }
    return false;
}