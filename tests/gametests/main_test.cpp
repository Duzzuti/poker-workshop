#include <gtest/gtest.h>

#include "logger.h"
#include "working_dir.h"

int main(int argc, char** argv) {
    srand(time(NULL));  // init random seed
    // the main function that is running the tests

    WorkingDir workingDir{argv[0], "log_gametest.txt"};

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(workingDir.getLogPath().c_str(), 1024 * 1024 * 10, 3);
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
    // plog::init(plog::verbose, &fileAppender);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
