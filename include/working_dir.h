#pragma once

#include <filesystem>

#include "logger.h"

/// @brief Provides functionalities for getting paths to the project directories
class WorkingDir {
   public:
    /// @brief Construct a new Working Dir object
    /// @param arg0 the first argument of the main function
    /// @param logFileName the name of the log file for this main method
    /// @exception Guarantee Strong
    /// @throws std::runtime_error if the project root directory could not be found
    /// @throws std::runtime_error if the data directory tree could not be created
    /// @note this constructor should be called in every main method of the project
    /// @note it will get the path to the executable and try to find the project root directory by going up the directory tree
    /// @note the project root directory is the directory containing the ".gitignore" file (might change this later)
    /// @note it will also set the paths to the log file, data directory, testscripts directory and gametests directory
    WorkingDir(const char* arg0, const std::string& logFileName) {
        // Get the path to the directory containing the executable
        this->exePath = std::filesystem::canonical(std::filesystem::path(arg0).parent_path());

        // search the project source dir
        this->projectRootPath = this->exePath;
        u_int8_t jumps = 0;
        // go up the directory tree until we find the project root directory with max 10 jumps
        while (!std::filesystem::exists(this->projectRootPath / ".gitignore") && jumps < 10) {
            this->projectRootPath = this->projectRootPath.parent_path();
            jumps++;
        }
        if (jumps == 10) {
            PLOG_FATAL << "Could not find project root directory";
            throw std::runtime_error("Could not find project root directory");
        }

        // add data directory if not already done
        if (!std::filesystem::exists(this->projectRootPath / "data")) {
            if (std::filesystem::create_directory(this->projectRootPath / "data"))
                PLOG_INFO << "Created data directory";
            else {
                PLOG_FATAL << "Could not create data directory. Check permissions";
                throw std::runtime_error("Could not create data directory. Check permissions");
            }
        }
        // add logs directory if not already done
        if (!std::filesystem::exists(this->projectRootPath / "data" / "logs")) {
            if (std::filesystem::create_directory(this->projectRootPath / "data" / "logs"))
                PLOG_INFO << "Created logs directory";
            else {
                PLOG_FATAL << "Could not create logs directory. Check permissions";
                throw std::runtime_error("Could not create logs directory. Check permissions");
            }
        }

        this->logPath = this->projectRootPath / "data" / "logs" / logFileName;
        this->dataPath = this->projectRootPath / "data";
        this->testscriptsPath = this->projectRootPath / "tests" / "testscripts";
        this->gametestsPath = this->projectRootPath / "tests" / "gametests";
    };

    /// @brief Get the path to the executable
    /// @return the path to the executable
    std::filesystem::path getExePath() const noexcept { return exePath; }

    /// @brief Get the path to the log file
    /// @return the path to the log file
    std::filesystem::path getLogPath() const noexcept { return logPath; }

    /// @brief Get the path to the data directory
    /// @return the path to the data directory
    std::filesystem::path getDataPath() const noexcept { return dataPath; }

    /// @brief Get the path to the testscripts directory
    /// @return the path to the testscripts directory
    std::filesystem::path getTestscriptsPath() const noexcept { return testscriptsPath; }

    /// @brief Get the path to the gametests directory
    /// @return the path to the gametests directory
    std::filesystem::path getGametestsPath() const noexcept { return gametestsPath; }

    /// @brief Get the path to the project root directory
    /// @return the path to the project root directory
    std::filesystem::path getProjectRootPath() const noexcept { return projectRootPath; }

   private:
    /// @brief the path to the executable
    std::filesystem::path exePath;
    /// @brief the path to the log file
    std::filesystem::path logPath;
    /// @brief the path to the data directory
    std::filesystem::path dataPath;
    /// @brief the path to the testscripts directory
    std::filesystem::path testscriptsPath;
    /// @brief the path to the gametests directory
    std::filesystem::path gametestsPath;
    /// @brief the path to the project root directory
    std::filesystem::path projectRootPath;
};