#include "config.h"
#include "game.h"
#include "logger.h"
#include "deck.h"

int main() {

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("data/log.txt", 1024 , 3);
    plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);

    PLOG_INFO << "Starting Application";

    Config config = BaseConfig();
    Game game{config};

    //game.run();

    Deck deck;
    deck.shuffle();
    std::cout << deck.toString();

    PLOG_INFO << "Finished Application";

    return 0;
}
