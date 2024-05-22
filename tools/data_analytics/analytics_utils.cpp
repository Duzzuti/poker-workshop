#include "analytics_utils.h"

#include <math.h>

#include <fstream>

void AnalyticsUtils::handStrengthsLoop(const std::filesystem::path& dataPath) noexcept {
    // set default options
    bool showFilterOptions[5] = {1, 0, 1, 1, 1};  // players, hand index, name, wins/total, wins/total*players
    bool filterPlayers[MAX_PLAYERS + 1];
    bool filterHands[HAND_INDEX_COUNT];

    std::memset(filterPlayers, 1, sizeof(filterPlayers));  // include all players
    std::memset(filterHands, 1, sizeof(filterHands));      // include all hands

    HandStrengthsOptions handStrengthsOptions;
    handStrengthsOptions.setRankType(true);                     // normalized values
    handStrengthsOptions.setFileName(1, 1, 1);                  // default is the winnerAdd = 1, splitAdd = 1, totalAdd = 1 file
    handStrengthsOptions.setShowOptions(showFilterOptions, 1);  // show differences
    handStrengthsOptions.setFilterOptions(filterPlayers, filterHands);

    // this loop runs until the user wants to quit
    while (true) {
        // 1. Print the current settings
        std::cout << "------------------------------------------------------------" << std::endl;
        std::cout << "Current settings:" << std::endl;
        std::cout << handStrengthsOptions << std::endl;
        std::cout << "------------------------------------------------------------" << std::endl;
        std::string input;
        int setting = -1;
        // 2. Ask the user for the setting he wants to change
        // This loop runs until the user enters a valid input
        while (true) {
            std::cout << "Enter the number of the setting you want to change or 0 to analyze the hand strengths ('q' to quit): ";
            std::cin >> input;
            // check if the user wants to quit
            if (input == "q") return;
            // check if the input is a valid number
            try {
                setting = std::stoi(input);
            } catch (std::invalid_argument& e) {
                std::cout << "Invalid input: " << input << std::endl;
                continue;
            }
            if (setting < 0 || setting > 5) {
                std::cout << "Invalid number: " << setting << std::endl;
                continue;
            }
            break;
        }
        // 3. Ask the user for the new setting they want to change
        if (setting == 1) {
            // new file name could be a string or 3 int options
            std::cout << "Enter the file name as a string with extension or enter the options separated by comma for the default name ('c' to cancel): ";
            std::string fileName;
            std::cin >> fileName;
            // check if the user wants to cancel
            if (fileName == "c") continue;
            // check if the user entered options
            if (fileName.find(',') != std::string::npos) {
                u_int8_t options[3];
                std::istringstream ss(fileName);
                std::string token;
                u_int8_t i = 0;
                // search for the 3 options (numbers between 0 and 9)
                while (std::getline(ss, token, ',') && i < 3) {
                    try {
                        int input = std::stoi(token);
                        if (input < 0 || input > 9) break;
                        options[i++] = input;
                    } catch (std::invalid_argument& e) {
                        break;
                    }
                }
                if (i != 3) {
                    std::cout << "Invalid options: " << fileName << std::endl;
                    continue;
                }
                handStrengthsOptions.setFileName(options);
            } else {
                handStrengthsOptions.setFileName(fileName);
            }
            continue;
        } else if (setting == 2) {
            // bool for the rank type
            std::cout << "Do you want to analyze the normalized values? (0/1) or (y/n) ('c' to cancel): ";
            std::cin >> input;
            // check if the user wants to cancel
            if (input == "c") continue;
            if (input == "0" || input == "1" || input == "y" || input == "n") {
                handStrengthsOptions.setRankType(input == "1" || input == "y");
            } else {
                std::cout << "Invalid input: " << input << std::endl;
            }
            continue;
        } else if (setting == 3) {
            // show options for the attributes and differences (bool array)
            std::cout << "Enter the options for the show settings (players, hand index, name, wins/total, wins/total*players, differences) as a comma separated list (0/1) or (y/n) ('c' to cancel): ";
            std::string showOptions;
            std::cin >> showOptions;
            // check if the user wants to cancel
            if (showOptions == "c") continue;
            std::istringstream ss(showOptions);
            std::string token;
            u_int8_t i = 0;
            bool options[5];
            bool showDifferences;
            // collect 5 bool options into the array and the last one is the differences
            while (std::getline(ss, token, ',') && i < 6) {
                if (token == "0" || token == "1" || token == "y" || token == "n") {
                    if (i < 5)
                        options[i] = token == "1" || token == "y";
                    else
                        showDifferences = token == "1" || token == "y";
                    i++;
                } else
                    break;
            }
            if (i != 6) {
                std::cout << "Invalid options: " << showOptions << std::endl;
                continue;
            }
            handStrengthsOptions.setShowOptions(options, showDifferences);
            continue;
        } else if (setting == 4) {
            // filter options for the players, the user has to specify whether he wants to select the included or excluded players
            std::cout << "Do you want to specify the players that should be included or excluded from the analysis? (i/e) ('c' to cancel): ";
            std::cin >> input;
            // check if the user wants to cancel
            if (input == "c") continue;
            if (input == "i" || input == "e") {
                // set all players to the opposite of the input (include/exclude)
                std::memset(filterPlayers, input == "e", sizeof(filterPlayers));
                std::cout << "Enter the players that should be included/excluded from the analysis as a comma separated list (2-" + std::to_string(+MAX_PLAYERS) + ") ('n' for None) ('c' to cancel): ";
                std::string players;
                std::cin >> players;
                // check if the user wants to cancel
                if (players == "c") continue;
                // check if the user wants to include/exclude no players
                if (players == "n") {
                    std::memset(filterPlayers, input == "e", sizeof(filterPlayers));
                    handStrengthsOptions.setFilterOptions(filterPlayers, filterHands);
                    continue;
                }
                std::istringstream ss(players);
                std::string token;
                bool error = false;
                // search for all numbers in the input, verify if they are valid and set the filterPlayers array
                while (std::getline(ss, token, ',')) {
                    try {
                        int player = std::stoi(token);
                        if (player < 2 || player > MAX_PLAYERS) {
                            std::cout << "Invalid player: " << player << std::endl;
                            error = true;
                            break;
                        }
                        filterPlayers[player] = input == "i";
                    } catch (std::invalid_argument& e) {
                        std::cout << "Invalid player: " << token << std::endl;
                        error = true;
                        break;
                    }
                }
                if (error) {
                    std::cout << "Invalid input: " << players << std::endl;
                    continue;
                }
                handStrengthsOptions.setFilterOptions(filterPlayers, filterHands);
            } else {
                std::cout << "Invalid input: " << input << std::endl;
            }
            continue;
        } else if (setting == 5) {
            // filter options for the hands, the user has to specify whether he wants to select the included or excluded hands
            std::cout << "Do you want to specify the hands that should be included or excluded from the analysis? (i/e) ('c' to cancel): ";
            std::cin >> input;
            // check if the user wants to cancel
            if (input == "c") continue;
            if (input == "i" || input == "e") {
                // set all hands to the opposite of the input (include/exclude)
                std::memset(filterHands, input == "e", sizeof(filterHands));
                std::cout << "Enter the hands that should be included/excluded from the analysis as a comma separated list (0-" + std::to_string(HAND_INDEX_COUNT - 1) +
                                 ") ('n' for None) ('c' to cancel): ";
                std::string hands;
                std::cin >> hands;
                // check if the user wants to cancel
                if (hands == "c") continue;
                // check if the user wants to include/exclude no hands
                if (hands == "n") {
                    std::memset(filterHands, input == "e", sizeof(filterHands));
                    handStrengthsOptions.setFilterOptions(filterPlayers, filterHands);
                    continue;
                }
                std::istringstream ss(hands);
                std::string token;
                bool error = false;
                // search for all numbers in the input, verify if they are valid and set the filterHands array
                while (std::getline(ss, token, ',')) {
                    try {
                        int hand = std::stoi(token);
                        if (hand < 0 || hand >= HAND_INDEX_COUNT) {
                            std::cout << "Invalid hand: " << hand << std::endl;
                            error = true;
                            break;
                        }
                        filterHands[hand] = input == "i";
                    } catch (std::invalid_argument& e) {
                        std::cout << "Invalid hand: " << token << std::endl;
                        error = true;
                        break;
                    }
                }
                if (error) {
                    std::cout << "Invalid input: " << hands << std::endl;
                    continue;
                }
                handStrengthsOptions.setFilterOptions(filterPlayers, filterHands);
            } else {
                std::cout << "Invalid input: " << input << std::endl;
            }
            continue;
        }

        // all settings do have a continue statement, this is implicitly reached if the user enters 0
        analyzeHandStrengths(dataPath, handStrengthsOptions);
    }
}

bool AnalyticsUtils::analyzeHandStrengths(const std::filesystem::path& dataPath, const HandStrengthsOptions& options) noexcept {
    // get the file
    std::string name = options.getFileName();
    const std::filesystem::path filePath = dataPath / name;
    if (!std::filesystem::exists(filePath)) {
        PLOG_WARNING << "File does not exist: " << filePath;
        return false;
    }
    // open the file
    std::ifstream file(filePath);
    if (!file.is_open()) {
        PLOG_WARNING << "Could not open file: " << filePath;
        return false;
    }
    // MAX_PLAYERS - 1 possible player configurations (2 - MAX_PLAYERS), HAND_INDEX_COUNT possible hands, 2 possible suited values
    HandStrengthsSet acceptedSets[(MAX_PLAYERS - 1) * HAND_INDEX_COUNT * 2];
    u_int32_t acceptedSetsCount = 0;
    // used to calculate the mean, sums all wins/total or wins/total*players values
    double sumWinsTotal = 0;
    // read the csv file
    std::string line;
    std::getline(file, line);  // skip the header
    // 1. Iterate over all datasets
    while (std::getline(file, line)) {
        HandStrengthsSet set{line};
        // 2. Filter out the datasets that are not relevant (filter options)
        if (!options.acceptSet(set)) continue;
        // 3. Add the dataset to the relevant datasets
        acceptedSets[acceptedSetsCount++] = set;
        sumWinsTotal += options.getNormalized() ? set.getWinsTotalPlayers() : set.getWinsTotal();
    }
    if (acceptedSetsCount == 0) {
        PLOG_WARNING << "No accepted sets found";
        return false;
    }
    // 4. Sort the datasets by the wins/total or wins/total*players values
    std::sort(acceptedSets, acceptedSets + acceptedSetsCount, [options](const HandStrengthsSet& a, const HandStrengthsSet& b) {
        return options.getNormalized() ? a.getWinsTotalPlayers() > b.getWinsTotalPlayers() : a.getWinsTotal() > b.getWinsTotal();
    });
    // 5. Calculate the mean, median, standard deviation with the relevant datasets and the normalized or non-normalized values (options)
    double mean = sumWinsTotal / acceptedSetsCount;
    double median;
    if (options.getNormalized())
        median = acceptedSetsCount % 2 == 0 ? (acceptedSets[acceptedSetsCount / 2].getWinsTotalPlayers() + acceptedSets[acceptedSetsCount / 2 - 1].getWinsTotalPlayers()) / 2
                                            : acceptedSets[acceptedSetsCount / 2].getWinsTotalPlayers();
    else
        median = acceptedSetsCount % 2 == 0 ? (acceptedSets[acceptedSetsCount / 2].getWinsTotal() + acceptedSets[acceptedSetsCount / 2 - 1].getWinsTotal()) / 2
                                            : acceptedSets[acceptedSetsCount / 2].getWinsTotal();
    double standardDeviation = 0;
    for (u_int32_t i = 0; i < acceptedSetsCount; i++) {
        double value = options.getNormalized() ? acceptedSets[i].getWinsTotalPlayers() : acceptedSets[i].getWinsTotal();
        standardDeviation += (value - mean) * (value - mean);
    }
    standardDeviation = std::sqrt(standardDeviation / acceptedSetsCount);
    // 6. Print meta data for the datasets
    const std::string strMean = options.getNormalized() ? "Normalized mean" : "Not-normalized mean";
    const std::string strMedian = options.getNormalized() ? "Normalized median" : "Not-normalized median";
    const std::string strStandardDeviation = options.getNormalized() ? "Normalized standard deviation" : "Not-normalized standard deviation";
    PLOG_INFO << "Accepted sets: " << acceptedSetsCount;
    PLOG_INFO << strMean << ": " << mean;
    PLOG_INFO << strMedian << ": " << median;
    PLOG_INFO << strStandardDeviation << ": " << standardDeviation;
    // 7. Add the differences to the mean and median if the option is set and print the data
    std::string header;
    if (options.getShowAttributes(0)) header += "Players, \t";
    if (options.getShowAttributes(1)) header += "Hand, \t";
    if (options.getShowAttributes(2)) header += "Name, \t";
    if (options.getShowAttributes(3)) header += "Wins/Total, \t";
    if (options.getShowAttributes(4)) header += "Wins/Total*Players, \t";
    if (options.getShowDifferences()) {
        header += strMean + " Difference, \t";
        header += strMedian + " Difference,";
    }
    std::cout << header << std::endl;
    for (u_int32_t i = 0; i < acceptedSetsCount; i++) {
        const std::string set = acceptedSets[i].getSetString(options.getNormalized(), options.getShowDifferences(), mean, median, standardDeviation, options.getShowAttributes());
        std::cout << set << std::endl;
    }

    file.close();

    return true;
}
