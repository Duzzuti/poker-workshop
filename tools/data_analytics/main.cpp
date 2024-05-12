#include "working_dir.h"

int main(int argc, char** argv) {
    srand(time(NULL));  // init random seed

    WorkingDir workingDir{argv[0], "log_tool.txt"};

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(workingDir.getLogPath().c_str(), 1024 * 1024 * 10, 5);
    // options
    bool defaultLogger = true;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            defaultLogger = false;
            plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
        } else if (strcmp(argv[i], "--silent") == 0) {
            defaultLogger = false;
        }
    }
    if(defaultLogger){
        // default to info level
        plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);
    }

    PLOG_INFO << "Starting DataAnalytics Tool";


    PLOG_INFO << "Finished DataAnalytics Tool";

    return 0;
}