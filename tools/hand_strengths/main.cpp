#include "deck.h"

int main(int argc, char** argv) {
    srand(time(NULL));  // init random seed
    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("log_tool.txt", 1024 * 1024 * 10, 5);
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
    PLOG_INFO << "Starting Tool";

    // TODO

    PLOG_INFO << "Finished Tool";

    return 0;
}