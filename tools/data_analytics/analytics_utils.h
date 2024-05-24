#pragma once

#include <filesystem>

#include "handstrengths_utils.h"

/// @brief API class containing functionality to analyze data
/// @note These container class functions are called to perform common analytic workflows
class AnalyticsUtils {
   public:
    /// @brief Starts a loop that will ask the user how he wants to analyze the hand strengths
    /// @param dataPath The path to the data directory
    /// @exception Guarantee No-throw
    /// @note The user can choose all available options like filtering, showing, source data file, etc.
    /// @see The choosen options will be passed to the analyzeHandStrengths() function
    static void handStrengthsLoop(const std::filesystem::path& dataPath) noexcept;
    /// @brief Analyzes the hand strengths with the given options
    /// @param dataPath The path to the data directory
    /// @param options The options to use for the analysis
    /// @return True if the analysis was successful, false otherwise
    /// @exception Guarantee No-throw
    /// @note The options contain all necessary information to filter, show, source file name, etc.
    /// @see handStrengthsLoop() if you want to ask the user for the options interactively
    static bool analyzeHandStrengths(const std::filesystem::path& dataPath, const HandStrengthsOptions& options) noexcept;
};