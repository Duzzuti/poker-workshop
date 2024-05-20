#include "analytics_utils.h"
#include "working_dir.h"

// TODO write wrapper which asks the user in a while loop which stats they want to see and call this script with the correct arguments

int main(int argc, char** argv) {
    srand(time(NULL));  // init random seed

    WorkingDir workingDir{argv[0], "log_tool.txt"};

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(workingDir.getLogPath().c_str(), 1024 * 1024 * 10, 5);
    // options
    bool defaultLogger = true;
    AnalyticsType analyticsType = AnalyticsType::NONE;

    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            defaultLogger = false;
            plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
        } else if (strcmp(argv[i], "--silent") == 0) {
            defaultLogger = false;
        } else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 < argc) {
                if (analyticsType != AnalyticsType::NONE) {
                    std::cerr << "Only one type of analytics can be selected at a time" << std::endl;
                    return 1;
                }
                if (strcmp(argv[i + 1], "hs") == 0) {
                    analyticsType = AnalyticsType::HANDSTRENGTHS;
                }
            }
        }
    }
    if (analyticsType == AnalyticsType::NONE) {
        std::cerr << "No analytics type selected" << std::endl;
        return 1;
    }
    if (defaultLogger) {
        // default to info level
        plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);
    }

    PLOG_INFO << "Starting DataAnalytics Tool";

    // start the selected analytics
    switch (analyticsType) {
        case AnalyticsType::HANDSTRENGTHS:
            PLOG_INFO << "Analyzing HandStrengths";
            AnalyticsUtils::handStrengthsLoop(workingDir.getDataPath());
            break;
        default:
            PLOG_ERROR << "Unknown analytics type";
            return 1;
    }

    PLOG_INFO << "Finished DataAnalytics Tool";

    return 0;
}