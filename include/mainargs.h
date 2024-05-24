#pragma once

#include <iostream>

#include "config.h"

/// @brief Represents one required main argument
/// @note The required arguments are positional and must be set in the correct order and before any optional arguments
/// @note Required arguments are without '-' or '--' prefix and are given directly after the program name
struct RequiredArg {
    /// @brief The "call"-name of the required argument; is used to identify the argument in the help texts
    std::string name;
    /// @brief The helper text for the required argument; is shown in the help text
    std::string helpText;
    /// @brief The value of the required argument; is set during the parsing of the arguments
    std::string value;
};

/// @brief Represents one optional main argument
/// @note The optional arguments are not positional and can be set in any order after the required arguments
/// @note Optional arguments can be flags (settable without a value), numeric or string values
struct Arg {
    /// @brief The short (single character) name of the argument; is used as "-<arg> [value]"
    char arg;
    /// @brief The long (multi character) name of the argument; is used as "--<longArg> [value]"
    std::string longArg;
    /// @brief The helper text for the argument; is shown in the help text
    std::string helpText;
    /// @brief If the argument is a numeric value
    bool isNumeric;
    /// @brief If the argument is a flag (settable without a value)
    bool isFlag;
    /// @brief The minimum value for the numeric argument
    int64_t min;
    /// @brief The maximum value for the numeric argument
    int64_t max;
    /// @brief The value of the argument; is set during the parsing of the arguments if the argument is not a flag
    std::string value{};
    /// @brief The numeric value of the argument; is set during the parsing of the arguments if the argument is not a string (flag gets 1)
    int64_t numericValue = 0;
};

/// @brief This class handles the functionality to parse the main arguments of a program
/// @note The class is used to set required and optional arguments and parse the given arguments
/// @note The class can generate a helper text and print the given arguments
/// @note The class encapsulates integrity checks for the given arguments
class MainArgs {
   public:
    /// @brief Constructor for the MainArgs class
    /// @param argc The number of arguments given to the program (directly from main)
    /// @param argv The array of arguments given to the program (directly from main)
    /// @exception Guarantee No-throw
    MainArgs(const int argc, const char** argv) noexcept : argc(argc), argv(argv){};

    /// @brief Set an required argument
    /// @param name The name of the required argument
    /// @param helpText The helper text for the required argument
    /// @return True if the argument was set successfully, false otherwise
    /// @exception Guarantee No-throw
    /// @note The required arguments are positional and must be set in the correct order (in the same order as they should be given to the program)
    /// @see MAX_REQUIRED_ARGS for the maximum number of required arguments
    /// @see setNumericArg() for setting a optional numeric argument
    /// @see setStringArg() for setting a optional string argument
    /// @see setFlag() for setting a optional flag argument
    bool setRequiredArg(const std::string& name, const std::string& helpText) noexcept {
        if (this->requiredArgCount < MAX_REQUIRED_ARGS) {
            this->requiredArgs[this->requiredArgCount].helpText = helpText;
            this->requiredArgs[this->requiredArgCount++].name = name;
            return true;
        }
        return false;
    }

    /// @brief Set an optional numeric argument
    /// @param arg The short (single character) name of the argument
    /// @param longArg The long (multi character) name of the argument
    /// @param min The minimum value for the numeric argument
    /// @param max The maximum value for the numeric argument
    /// @param helpText The helper text for the argument
    /// @return True if the argument was set successfully, false otherwise
    /// @exception Guarantee No-throw
    /// @see MAX_OPTIONAL_ARGS for the maximum number of optional arguments
    /// @see setFlag() for setting a optional flag argument
    /// @see setStringArg() for setting a optional string argument
    /// @see setRequiredArg() for setting a required argument
    bool setNumericArg(const char arg, const std::string& longArg, const int64_t min, const int64_t max, const std::string& helpText) noexcept {
        Arg argStruct;
        argStruct.arg = arg;
        argStruct.longArg = longArg;
        argStruct.isNumeric = true;
        argStruct.isFlag = false;
        argStruct.min = min;
        argStruct.max = max;
        argStruct.helpText = helpText;
        return this->setArg(argStruct);
    }

    /// @brief Set an optional string argument
    /// @param arg The short (single character) name of the argument
    /// @param longArg The long (multi character) name of the argument
    /// @param helpText The helper text for the argument
    /// @return True if the argument was set successfully, false otherwise
    /// @exception Guarantee No-throw
    /// @see MAX_OPTIONAL_ARGS for the maximum number of optional arguments
    /// @see setFlag() for setting a optional flag argument
    /// @see setNumericArg() for setting a optional numeric argument
    /// @see setRequiredArg() for setting a required argument
    bool setStringArg(const char arg, const std::string& longArg, const std::string& helpText) noexcept {
        Arg argStruct;
        argStruct.arg = arg;
        argStruct.longArg = longArg;
        argStruct.isNumeric = false;
        argStruct.isFlag = false;
        argStruct.helpText = helpText;
        return this->setArg(argStruct);
    }

    /// @brief Set an optional flag argument
    /// @param arg The short (single character) name of the argument
    /// @param longArg The long (multi character) name of the argument
    /// @param helpText The helper text for the argument
    /// @return True if the argument was set successfully, false otherwise
    /// @exception Guarantee No-throw
    /// @see MAX_OPTIONAL_ARGS for the maximum number of optional arguments
    /// @see setStringArg() for setting a optional string argument
    /// @see setNumericArg() for setting a optional numeric argument
    /// @see setRequiredArg() for setting a required argument
    bool setFlag(const char arg, const std::string& longArg, const std::string& helpText) noexcept {
        Arg argStruct;
        argStruct.arg = arg;
        argStruct.longArg = longArg;
        argStruct.isNumeric = false;
        argStruct.isFlag = true;
        argStruct.helpText = helpText;
        return this->setArg(argStruct);
    }

    /// @brief Get the help text for this main argument parser
    /// @return The formatted help text
    /// @exception Guarantee No-throw
    /// @note Creates a formatted help text with the description, usage, required arguments and optional arguments help texts
    std::string getHelpText() const noexcept {
        std::string helpText = "Description: \n" + this->description + "\n";
        helpText += "Usage: \t" + std::string(this->argv[0]) + " ";
        for (u_int8_t i = 0; i < this->requiredArgCount; i++) {
            helpText += "<" + this->requiredArgs[i].name + "> ";
        }
        helpText += "[options]\n";
        // required arguments
        if (this->requiredArgCount > 0) {
            helpText += "Required arguments: \n";
            for (u_int8_t i = 0; i < this->requiredArgCount; i++) {
                helpText += "\t<" + this->requiredArgs[i].name + ">\n";
                helpText += "\t\t" + this->requiredArgs[i].helpText + "\n";
            }
        }
        // optional arguments
        helpText += "Options: \n";
        helpText += "\t\t--help\n";
        helpText += "\t\tShow this help text\n";
        for (u_int8_t i = 0; i < this->argCount; i++) {
            helpText += "\t-" + std::string() + this->args[i].arg + ", \t--" + this->args[i].longArg + "\n";
            helpText += "\t\t" + this->args[i].helpText + "\n";
        }
        return helpText;
    }

    /// @brief Parse the given arguments
    /// @return True if the arguments were parsed successfully, false otherwise
    /// @exception Guarantee No-throw
    /// @note The function sets the values of the arguments and checks if all required arguments are set
    /// @note The function checks if an argument is set multiple times
    /// @note The function checks if the given values for the arguments are in the correct range
    /// @note The function checks for invalid arguments
    /// @note The function checks for "--help" argument and shows the help text if set
    /// @note The function shows the help text if an argument is missing or invalid
    bool run() noexcept {
        // showHelp == true if an error occurred => show help text and return false
        bool showHelp = false;
        // check for required arguments
        for (u_int8_t i = 0; i < this->requiredArgCount; i++) {
            // check if there are enough arguments and the argument is not a flag
            if (this->argc > i + 1 && this->argv[i + 1][0] != '-') {
                this->requiredArgs[i].value = this->argv[i + 1];
            } else {
                std::cerr << "Missing required argument: " << this->requiredArgs[i].name << std::endl;
                showHelp = true;
            }
        }
        if (!showHelp) {
            // check for optional arguments (do not break on all errors, show multiple errors at once)
            for (int i = this->requiredArgCount + 1; i < this->argc; i++) {
                for (u_int8_t j = 0; j < this->argCount; j++) {
                    // check if the argument is valid
                    if (std::string() + this->argv[i] == std::string("-") + this->args[j].arg || std::string() + this->argv[i] == std::string("--") + this->args[j].longArg) {
                        // check for duplicate arguments
                        if (this->isArgSet(j)) {
                            std::cerr << "Argument " << this->args[j].longArg << " already set" << std::endl;
                            showHelp = true;
                            // skip the value for non-flags
                            if (!this->args[j].isFlag) i++;
                            continue;
                        }
                        // check if the argument is a flag and set numeric value to 1
                        if (this->args[j].isFlag) this->args[j].numericValue = 1;
                        // check if enough arguments are given for the value
                        else if (i + 1 < this->argc) {
                            i++;  // skip the value in the next iteration
                            this->args[j].value = this->argv[i];
                            // check numeric content and bounds
                            if (this->args[j].isNumeric) {
                                int64_t value;
                                try {
                                    value = std::stoll(this->argv[i]);
                                } catch (const std::invalid_argument& e) {
                                    std::cerr << "Value for " << this->args[j].longArg << " must be a number. Got: " << this->argv[i] << std::endl;
                                    showHelp = true;
                                }
                                if (value >= this->args[j].min && value <= this->args[j].max)
                                    this->args[j].numericValue = value;
                                else {
                                    std::cerr << "Value for " << this->args[j].longArg << " must be between " << this->args[j].min << " and " << this->args[j].max << ". Got: " << this->argv[i]
                                              << std::endl;
                                    showHelp = true;
                                }
                            }
                        } else {
                            std::cerr << "No value for " << this->args[j].longArg << " provided" << std::endl;
                            showHelp = true;
                        }
                        break;  // Argument was found and set
                        // check for help argument
                    } else if (std::strncmp(this->argv[i], "--help", 7) == 0) {
                        showHelp = true;
                        break;
                    } else if (j == this->argCount - 1) {
                        std::cerr << "Unknown argument: " << this->argv[i] << std::endl;
                        showHelp = true;
                        break;
                    }
                }
            }
        }
        // show help text if an error occurred
        if (showHelp) {
            std::cout << this->getHelpText() << std::endl;
            return false;
        }
        return true;
    }

    /// @brief Print the arguments with their given values
    /// @exception Guarantee No-throw
    /// @note Requires the run() function to be called before
    void printGotArgs() const noexcept {
        for (u_int8_t i = 0; i < this->argCount; i++) {
            if (this->args[i].isFlag) {
                std::cout << this->args[i].longArg << ": " << (this->args[i].numericValue == 1 ? "true" : "false") << std::endl;
            } else if (this->args[i].isNumeric) {
                std::cout << this->args[i].longArg << ": " << this->args[i].numericValue << std::endl;
            } else {
                std::cout << this->args[i].longArg << ": " << this->args[i].value << std::endl;
            }
        }
    }

    /// @brief Check if an optional argument is set
    /// @param index The index of the optional argument (in the order they were set)
    /// @return True if the argument is set, false otherwise
    /// @exception Guarantee No-throw
    /// @note Requires the run() function to be called before
    bool isArgSet(const u_int index) const noexcept { return this->args[index].isFlag ? this->args[index].numericValue == 1 : !this->args[index].value.empty(); }

    /// @brief Get the value of an optional argument
    /// @param index The index of the optional argument (in the order they were set)
    /// @return The string value of the argument
    /// @exception Guarantee No-throw
    /// @note Requires the run() function to be called before
    std::string getArgValue(const u_int index) const noexcept { return this->args[index].value; }

    /// @brief Append a description line to the general help text
    /// @param description The description line to append
    /// @exception Guarantee No-throw
    /// @note The description line is indented with a tab and a newline is appended
    void appendDescriptionLine(const std::string& description) noexcept { this->description += "\t" + description + "\n"; }

   private:
    /// @brief The amount of arguments given to the program (directly from main)
    const int argc;
    /// @brief The array of arguments given to the program (directly from main)
    const char** argv;

    /// @brief The general description for the main parser and program
    std::string description;

    /// @brief The array of optional arguments that could be set
    Arg args[MAX_OPTIONAL_ARGS];
    /// @brief The amount of optional arguments that could be set
    u_int8_t argCount = 0;

    /// @brief The array of required arguments that must be set
    RequiredArg requiredArgs[MAX_REQUIRED_ARGS];
    /// @brief The amount of required arguments that must be set
    u_int8_t requiredArgCount = 0;

    /// @brief Set an optional argument
    /// @param arg The argument to set
    /// @return True if the argument was set successfully, false otherwise
    /// @exception Guarantee No-throw
    /// @see MAX_OPTIONAL_ARGS for the maximum number of optional arguments
    /// @see setNumericArg() for setting a optional numeric argument
    /// @see setStringArg() for setting a optional string argument
    /// @see setFlag() for setting a optional flag argument
    /// @see setRequiredArg() for setting a required argument
    bool setArg(const Arg& arg) noexcept {
        if (argCount < MAX_OPTIONAL_ARGS) {
            this->args[this->argCount++] = arg;
            return true;
        }
        return false;
    }

    /// @brief Check if a given argument (through the main function) is a true value
    /// @param index The index of the argument in the main function arguments
    /// @return True if the argument is a true value, false otherwise
    /// @exception Guarantee No-throw
    /// @note The true values are: "1", "y", "t", "true"
    bool isArgTrueValue(const u_int index) const noexcept {
        // do a save string compare
        return (std::strncmp(this->argv[index], "1", 2) == 0 || std::strncmp(this->argv[index], "y", 2) == 0 || std::strncmp(this->argv[index], "t", 2) == 0 ||
                std::strncmp(this->argv[index], "true", 5) == 0);
    }

    /// @brief Check if a given argument (through the main function) is a false value
    /// @param index The index of the argument in the main function arguments
    /// @return True if the argument is a false value, false otherwise
    /// @exception Guarantee No-throw
    /// @note The false values are: "0", "n", "f", "false"
    bool isArgFalseValue(const u_int index) const noexcept {
        // do a save string compare
        return (std::strncmp(this->argv[index], "0", 2) == 0 || std::strncmp(this->argv[index], "n", 2) == 0 || std::strncmp(this->argv[index], "f", 2) == 0 ||
                std::strncmp(this->argv[index], "false", 6) == 0);
    }
};