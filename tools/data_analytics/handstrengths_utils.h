#pragma once

#include "config.h"

/// @brief This struct represents a single hand strength data set
struct HandStrengthsSet {
   public:
    /// @brief Construct a HandStrengthsSet from a csv string (from a data file)
    /// @param set The csv string to parse
    /// @exception Guarantee None
    /// @throws std::invalid_argument If the set is invalid
    /// @note The set has the format "Players, Hand, Suited, Name, Wins, Total, Wins/Total, Wins/Total*Players,"
    /// @see HandUtils for creating the data files
    HandStrengthsSet(const std::string& set) {
        // Set has the format "Players, Hand, Suited, Name, Wins, Total, Wins/Total, Wins/Total*Players,"
        // find all commas and check if the values are valid, ignore unimportant values
        const size_t firstComma = set.find(",");
        const size_t secondComma = set.find(", ", firstComma + 2);
        const size_t thirdComma = set.find(", ", secondComma + 2);
        const size_t fourthComma = set.find(", ", thirdComma + 2);
        const size_t fifthComma = set.find(", ", fourthComma + 2);
        const size_t sixthComma = set.find(", ", fifthComma + 2);
        const size_t seventhComma = set.find(", ", sixthComma + 2);
        const size_t eighthComma = set.find(", ", seventhComma + 2);
        const int tPlayers = std::stoi(set.substr(0, firstComma));
        if (tPlayers > MAX_PLAYERS || tPlayers < 2) {
            PLOG_ERROR << "Invalid set: " << set << " (players: " << tPlayers << ")";
            throw std::invalid_argument("Invalid set");
        }
        this->players = tPlayers;
        const int tHandIndex = std::stoi(set.substr(firstComma + 2, secondComma - firstComma - 2));
        if (tHandIndex >= HAND_INDEX_COUNT || tHandIndex < 0) {
            PLOG_ERROR << "Invalid set: " << set << " (hand index: " << tHandIndex << ")";
            throw std::invalid_argument("Invalid set");
        }
        this->handIndex = tHandIndex;
        const std::string tName = set.substr(thirdComma + 2, fourthComma - thirdComma - 2);
        if (tName.length() != 3) {
            PLOG_ERROR << "Invalid name: " << tName;
            throw std::invalid_argument("Invalid set");
        }
        std::memcpy(this->name, tName.c_str(), 3);
        this->name[3] = '\0';
        const double tWinsTotal = std::stod(set.substr(sixthComma + 2, seventhComma - sixthComma - 2));
        if (tWinsTotal < 0) {
            PLOG_ERROR << "Invalid set: " << set << " (wins total: " << tWinsTotal << ")";
            throw std::invalid_argument("Invalid set");
        }
        this->winsTotal = tWinsTotal;
        const double tWinsTotalPlayers = std::stod(set.substr(seventhComma + 2, eighthComma - seventhComma - 2));
        if (tWinsTotalPlayers < 0) {
            PLOG_ERROR << "Invalid set: " << set << " (wins total players: " << tWinsTotalPlayers << ")";
            throw std::invalid_argument("Invalid set");
        }
        this->winsTotalPlayers = tWinsTotalPlayers;
    };

    /// @brief Construct a HandStrengthsSet with default (invalid) values
    /// @exception Guarantee No-throw
    /// @note This constructor is needed to initialize an array of HandStrengthsSet
    HandStrengthsSet() noexcept {
        this->players = 0;
        this->handIndex = 0;
        this->name[0] = 'I';
        this->name[1] = 'n';
        this->name[2] = 'v';
        this->name[3] = '\0';
        this->winsTotal = 0;
        this->winsTotalPlayers = 0;
    }

    /// @brief Getter for the players attribute of the set
    /// @return The number of players for this set
    /// @exception Guarantee No-throw
    unsigned char getPlayers() const noexcept { return this->players; };

    /// @brief Getter for the hand index attribute of the set
    /// @return The index of the hand for this set
    /// @exception Guarantee No-throw
    unsigned char getHandIndex() const noexcept { return this->handIndex; };

    /// @brief Getter for the hand name attribute of the set
    /// @return The name of the hand for this set
    /// @exception Guarantee No-throw
    const char* getName() const noexcept { return this->name; };

    /// @brief Getter for the wins/total attribute of the set
    /// @return The win rate of the hand for this set
    /// @exception Guarantee No-throw
    double getWinsTotal() const noexcept { return this->winsTotal; };

    /// @brief Getter for the wins/total*players attribute of the set
    /// @return The normalized win rate of the hand for this set
    /// @exception Guarantee No-throw
    double getWinsTotalPlayers() const noexcept { return this->winsTotalPlayers; };

    /// @brief Get a string representation of the set with the given filters
    /// @param normalized Whether the normalized values should be used for the differences (mean, median, standard deviation)
    /// @param showDiff Whether the differences from the mean and median should be shown
    /// @param mean The mean value for the differences
    /// @param median The median value for the differences
    /// @param standardDeviation The standard deviation value for the differences
    /// @param filters The show filters for the attributes (players, hand index, name, wins/total, wins/total*players)
    /// @return The string representation of the set with the given filters
    /// @exception Guarantee No-throw
    std::string getSetString(const bool normalized, const bool showDiff, const double mean, const double median, const double standardDeviation, const bool filters[5]) const noexcept {
        // Add the attributes to the set string which are set to be shown
        std::string set;
        if (filters[0]) set += std::to_string(this->players) + ", \t\t";
        if (filters[1]) set += std::to_string(this->handIndex) + ", \t";
        if (filters[2]) set += std::string(this->name, 3) + ", \t";
        if (filters[3]) set += this->limitDecimals(this->winsTotal) + ", \t\t";
        if (filters[4]) set += this->limitDecimals(this->winsTotalPlayers) + ", \t\t\t";
        // Add the differences to the set string if they are set to be shown (show them absolute and relative to the standard deviation)
        if (showDiff) {
            if (normalized) {
                set += this->limitDecimals(this->addSign(this->winsTotalPlayers - mean)) + "(" + this->limitDecimals(this->addSign((this->winsTotalPlayers - mean) / standardDeviation)) + "σ), \t\t";
                set += this->limitDecimals(this->addSign(this->winsTotalPlayers - median)) + "(" + this->limitDecimals(this->addSign((this->winsTotalPlayers - median) / standardDeviation)) + "σ), ";
            } else {
                set += this->limitDecimals(this->addSign(this->winsTotal - mean)) + "(" + this->limitDecimals(this->addSign((this->winsTotal - mean) / standardDeviation)) + "σ), \t\t";
                set += this->limitDecimals(this->addSign(this->winsTotal - median)) + "(" + this->limitDecimals(this->addSign((this->winsTotal - median) / standardDeviation)) + "σ), ";
            }
        }
        return set;
    }

    /// @brief Print the set (with all saved attributes) to the output stream (csv format)
    /// @param os The output stream to print to
    /// @param set The set to print
    /// @return The output stream with the set printed to it
    /// @exception Guarantee No-throw
    /// @note The printed string is NOT the same as the getSetString() function and also NOT the same as the string given to the constructor
    friend std::ostream& operator<<(std::ostream& os, const HandStrengthsSet& set) noexcept {
        os << +set.players << ", " << +set.handIndex << ", " << set.name << ", " << set.winsTotal << ", " << set.winsTotalPlayers;
        return os;
    }

   private:
    /// @brief The number of players for this set
    unsigned char players;
    /// @brief The index of the hand for this set
    unsigned char handIndex;
    /// @brief The name of the hand for this set
    char name[4];  // 3 characters + null terminator
    /// @brief The win rate of the hand for this set
    double winsTotal;
    /// @brief The normalized win rate of the hand for this set
    double winsTotalPlayers;

    /// @brief Add a sign to the number (positive numbers get a "+" sign)
    /// @param number The number to add the sign to
    /// @return The number as a string with the sign
    /// @exception Guarantee No-throw
    const std::string addSign(const double number) const noexcept { return number >= 0 ? "+" + std::to_string(number) : std::to_string(number); }

    /// @brief Limit the number of decimals of a double number (cut off the rest, no rounding)
    /// @param number The number to limit the decimals of
    /// @param places The number of decimals to keep
    /// @return The number as a string with the limited decimals
    /// @exception Guarantee No-throw
    /// @see limitDecimals(const std::string&, const u_int8_t) for limiting the decimals of a string number
    const std::string limitDecimals(const double number, const u_int8_t places = 3) const noexcept {
        std::string str = std::to_string(number);
        return str.substr(0, str.find('.') + places + 1);
    }

    /// @brief Limit the number of decimals of a string number (cut off the rest, no rounding)
    /// @param str The string number to limit the decimals of
    /// @param places The number of decimals to keep
    /// @return The string number with the limited decimals
    /// @exception Guarantee No-throw
    /// @see limitDecimals(const double, const u_int8_t) for limiting the decimals of a double number
    const std::string limitDecimals(const std::string& str, const u_int8_t places = 3) const noexcept { return str.substr(0, str.find('.') + places + 1); }
};

/// @brief Represents the options for the hand strengths analysis
class HandStrengthsOptions {
   public:
    /// @brief Setter for the file name
    /// @param fileName The name of the file to use for the hand strengths data files
    /// @exception Guarantee No-throw
    /// @see setFileName(const u_int8_t[3]) for setting the file name with options
    /// @see setFileName(const u_int8_t, const u_int8_t, const u_int8_t) for setting the file name with options
    void setFileName(const std::string& fileName) noexcept { this->fileName = fileName; };

    /// @brief Setter for the file name with options
    /// @param options These are appended to the default file name (winnerAdd, splitAdd, totalAdd)
    /// @exception Guarantee No-throw
    /// @note The name is constructed using the default name and the options
    /// @see STR_TOOL_HANDSTRENGTHS_DEFAULT_FILE_NAME for the default file name
    /// @see HandUtils for the creation of the data files
    /// @see setFileName(const std::string&) for setting the file name with a custom name
    /// @see setFileName(const u_int8_t, const u_int8_t, const u_int8_t) for setting the file name with options
    void setFileName(const u_int8_t options[3]) noexcept;

    /// @brief Setter for the file name with options
    /// @param winnerAdd The amount to add to the win stat if the hand is the only winner
    /// @param splitAdd The amount to add to the win stat if the hand is a split winner
    /// @param totalAdd The amount to add to the total stat for each occurrence
    /// @exception Guarantee No-throw
    /// @note The name is constructed using the default name and the options
    /// @see STR_TOOL_HANDSTRENGTHS_DEFAULT_FILE_NAME for the default file name
    /// @see HandUtils for the creation of the data files
    /// @see setFileName(const std::string&) for setting the file name with a custom name
    /// @see setFileName(const u_int8_t[3]) for setting the file name with options
    void setFileName(const u_int8_t winnerAdd, const u_int8_t splitAdd, const u_int8_t totalAdd) noexcept;

    /// @brief Setter for the normalized attribute
    /// @param normalized Whether the normalized values should be used for the ranking
    /// @exception Guarantee No-throw
    void setRankType(const bool normalized) noexcept { this->normalized = normalized; };

    /// @brief Setter for the show attributes and differences flags
    /// @param showAttributes The attributes to show (players, hand index, name, wins/total, wins/total*players)
    /// @param showDifferences Whether the differences (absolute, relative and standard deviations) from the mean and median should be shown for each hand
    /// @exception Guarantee No-throw
    void setShowOptions(const bool showAttributes[5], const bool showDifferences) noexcept;

    /// @brief Setter for the filter options
    /// @param filterPlayers The players to filter (true if included, false if excluded)
    /// @param filterHands The hands to filter (true if included, false if excluded)
    /// @exception Guarantee No-throw
    void setFilterOptions(const bool filterPlayers[MAX_PLAYERS + 1], const bool filterHands[HAND_INDEX_COUNT]) noexcept;

    /// @brief Check if the set is accepted by the set filters
    /// @param set The set to check
    /// @return True if the set is accepted, false otherwise
    /// @exception Guarantee No-throw
    /// @note Data file header has to be skipped
    /// @note Undefined behavior if the set or the filters are invalid (not set correctly)
    /// @see HandStrengthsSet for the format of the set
    bool acceptSet(const HandStrengthsSet& set) const noexcept;

    /// @brief Getter for the file name
    /// @return The name of the file to be used for hand strengths data files
    /// @exception Guarantee No-throw
    const std::string getFileName() const noexcept { return this->fileName; };

    /// @brief Getter for the normalized attribute
    /// @return Whether the normalized values should be used for the ranking
    /// @exception Guarantee No-throw
    bool getNormalized() const noexcept { return this->normalized; };

    /// @brief Getter for the show attributes flags
    /// @param index The index of the attribute to get (players, hand index, name, wins/total, wins/total*players)
    /// @return Whether the specific attribute should be shown
    /// @exception Guarantee No-throw
    bool getShowAttributes(const int index) const noexcept { return this->showAttributes[index]; };

    /// @brief Getter for the show attributes flags array
    /// @return Array of the show attributes flags (players, hand index, name, wins/total, wins/total*players)
    /// @exception Guarantee No-throw
    const bool* getShowAttributes() const noexcept { return this->showAttributes; };

    /// @brief Getter for the show differences flag
    /// @return Whether the differences (absolute, relative and standard deviations) from the mean and median should be shown for each hand
    /// @exception Guarantee No-throw
    bool getShowDifferences() const noexcept { return this->showDifferences; };

    /// @brief Overload of the << operator to print the options to the output stream
    /// @param os The output stream to print to
    /// @param options The options to print
    /// @return The output stream with the options printed to it
    /// @exception Guarantee No-throw
    /// @note This shows the set options in a numbered list format for the user to understand
    /// @note The options are shown in the following order: file name, normalized, show attributes, filter players, filter hands
    /// @note The filter players and hands are shown with the majority setting (included or excluded) to make it easier to understand
    friend std::ostream& operator<<(std::ostream& os, const HandStrengthsOptions& options) noexcept {
        os << "1) Filename: " << options.fileName << std::endl;
        os << "2) Normalized: " << (options.normalized ? "true" : "false") << std::endl;
        os << "3) Show attributes (players, hand index, name, wins/total, wins/total*players, differences): ";
        for (u_int8_t i = 0; i < 6; i++) {
            if (i == 5)
                os << (options.showDifferences ? "true " : "false ") << std::endl;
            else
                os << (options.showAttributes[i] ? "true, " : "false, ");
        }
        bool firstPlayer = true;
        os << "4) Filter players ";
        // if the majority is not set, show the included players, otherwise show the excluded players
        if (!options.filterPlayerMajoritySet) {
            os << "(INCLUDED players): ";
            for (u_int8_t i = 2; i <= MAX_PLAYERS; i++) {
                if (options.filterPlayers[i]) {
                    if (firstPlayer)
                        firstPlayer = false;
                    else
                        os << ", ";
                    os << +i;
                }
            }
        } else {
            os << "(EXCLUDED players): ";
            for (u_int8_t i = 2; i <= MAX_PLAYERS; i++) {
                if (!options.filterPlayers[i]) {
                    if (firstPlayer)
                        firstPlayer = false;
                    else
                        os << ", ";
                    os << +i;
                }
            }
        }
        // if no players are included/excluded, show "None"
        if (firstPlayer) os << "None";
        bool firstHand = true;
        os << std::endl << "5) Filter hands ";
        // if the majority is not set, show the included hands, otherwise show the excluded hands
        if (!options.filterHandMajoritySet) {
            os << "(INCLUDED hands): ";
            for (u_int8_t i = 0; i < HAND_INDEX_COUNT; i++) {
                if (options.filterHands[i]) {
                    if (firstHand)
                        firstHand = false;
                    else
                        os << ", ";
                    os << +i;
                }
            }
        } else {
            os << "(EXCLUDED hands): ";
            for (u_int8_t i = 0; i < HAND_INDEX_COUNT; i++) {
                if (!options.filterHands[i]) {
                    if (firstHand)
                        firstHand = false;
                    else
                        os << ", ";
                    os << +i;
                }
            }
        }
        // if no hands are included/excluded, show "None"
        if (firstHand) os << "None";
        return os;
    }

   private:
    /// @brief name of the file to be used for hand strengths data files
    std::string fileName;
    /// @brief Whether the normalized values should be used for the ranking
    bool normalized;
    /// @brief Whether the specific attributes should be shown (players, hand index, name, wins/total, wins/total*players)
    bool showAttributes[5];
    /// @brief Whether the differences (absolute, relative and standard deviations) from the mean and median should be shown for each hand
    bool showDifferences;

    /// @brief True if the player count should be included in the analysis
    bool filterPlayers[MAX_PLAYERS + 1];
    /// @brief True if the hand index should be included in the analysis
    bool filterHands[HAND_INDEX_COUNT];

    /// @brief Whether the majority of the players should be included in the analysis
    bool filterPlayerMajoritySet;
    /// @brief Whether the majority of the hands should be included in the analysis
    bool filterHandMajoritySet;
};