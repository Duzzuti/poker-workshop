#include "game.h"
#include "working_dir.h"

int main(int argc, char** argv) {
    srand(time(NULL));  // init random seed

    WorkingDir workingDir{argv[0], "log_game.txt"};

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(workingDir.getLogPath().c_str(), 1024 * 1024 * 10, 5);
    // plog::init(plog::verbose, &fileAppender);
    // plog::init(plog::verbose, &consoleAppender);
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
            break;
        }
        if (strcmp(argv[i], "-i") == 0) {
            plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);
            break;
        }
    }
    PLOG_INFO << "Starting Application";

    Config config = BaseConfig();
    Game game{config};

    game.run();

    PLOG_INFO << "Finished Application";

    return 0;
}
