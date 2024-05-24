#include "game.h"
#include "mainargs.h"
#include "working_dir.h"

int main(const int argc, const char** argv) {
    srand(time(NULL));  // init random seed

    WorkingDir workingDir{argv[0], "log_game.txt"};
    MainArgs mainArgs(argc, argv);
    mainArgs.appendDescriptionLine("Poker Game Simulator");
    mainArgs.appendDescriptionLine("At default there is no logging, use -v for verbose logging and -i for info logging");
    mainArgs.setFlag('v', "verbose", "Enable verbose logging");
    mainArgs.setFlag('i', "info", "Enable info logging");
    if (!mainArgs.run()) return 1;

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(workingDir.getLogPath().c_str(), 1024 * 1024 * 10, 5);

    // verbose logging
    if (mainArgs.isArgSet(0)) {
        if (mainArgs.isArgSet(1)) {
            std::cerr << "Cannot set both verbose and info logging arguments" << std::endl;
            return 1;
        }
        plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
    }
    // info logging
    if (mainArgs.isArgSet(1)) {
        plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);
    }

    PLOG_INFO << "Starting Application";

    Config config = BaseConfig();
    Game game{config};

    game.run();

    PLOG_INFO << "Finished Application";

    return 0;
}
