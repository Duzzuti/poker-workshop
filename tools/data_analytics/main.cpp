#include "analytics_utils.h"
#include "mainargs.h"
#include "working_dir.h"

// TODO write wrapper which asks the user in a while loop which stats they want to see and call this script with the correct arguments

int main(const int argc, const char** argv) {
    srand(time(NULL));  // init random seed

    WorkingDir workingDir{argv[0], "log_tool.txt"};
    MainArgs mainArgs(argc, argv);
    mainArgs.appendDescriptionLine("DataAnalytics Tool");
    mainArgs.appendDescriptionLine("This tool is used to analyze data produced from other tools and saved in the data directory");
    mainArgs.appendDescriptionLine("The type of analytics to perform has to be selected with the -t argument");
    mainArgs.setFlag('v', "verbose", "Enable verbose logging");
    mainArgs.setFlag('s', "silent", "Disable logging");
    mainArgs.setStringArg('t', "type", "The type of analytics to perform. Possible values: 'hs' (handstrengths)");
    if (!mainArgs.run()) return 1;

    mainArgs.printGotArgs();
    std::cout << mainArgs.getHelpText() << std::endl;

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(workingDir.getLogPath().c_str(), 1024 * 1024 * 10, 5);
    // options
    bool defaultLogger = true;

    // verbose logging
    if (mainArgs.isArgSet(0)) {
        defaultLogger = false;
        plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
    }
    // silent logging
    if (mainArgs.isArgSet(1)) {
        if (!defaultLogger) {
            std::cerr << "Cannot set both verbose and silent logging arguments" << std::endl;
            return 1;
        }
        defaultLogger = false;
    }
    // default logger to info level
    if (defaultLogger) plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);

    PLOG_INFO << "Starting DataAnalytics Tool";

    // analytics type
    if (mainArgs.isArgSet(2)) {
        // start the selected analytics
        if (mainArgs.getArgValue(2) == "hs") {
            PLOG_INFO << "Analyzing HandStrengths";
            AnalyticsUtils::handStrengthsLoop(workingDir.getDataPath());
        } else {
            std::cerr << "Unknown analytics type: " << mainArgs.getArgValue(2) << ". --help for more information" << std::endl;
            return 1;
        }
    } else {
        std::cerr << "No analytics type selected. --help for more information" << std::endl;
        return 1;
    }

    PLOG_INFO << "Finished DataAnalytics Tool";

    return 0;
}