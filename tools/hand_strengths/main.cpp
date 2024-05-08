#include "hand_utils.h"

int main(int argc, char** argv) {
    srand(time(NULL));  // init random seed
    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender("log_tool.txt", 1024 * 1024 * 10, 5);
    // options
    u_int64_t iters = 10000000;
    std::string filename = "hand_strengths.csv";
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0) {
            plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
        } else if (strcmp(argv[i], "-i") == 0) {
            plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);
        } else if (strcmp(argv[i], "--iters") == 0) {
            iters = std::stoull(argv[i + 1]);
        } else if (strcmp(argv[i], "-o") == 0) {
            std::cout << "Output file: " << argv[i + 1] << std::endl;
            filename = argv[i + 1];
        }
    }
    PLOG_INFO << "Starting Handstrengths Tool";

    Deck deck;
    Card communityCards[5];
    std::pair<Card, Card> playerCards[MAX_PLAYERS];
    // iterate over any meaningful number of players
    for (u_int8_t players = 2; players <= MAX_PLAYERS; players++) {
        // set up HandUtils // TODO: set custom (main-)options
        HandUtils handUtils(1, 1, 1);
        // simulate for an amount of iterations
        for (u_int64_t i = 0; i < iters; i++) {
            // shuffle deck and draw cards
            deck.shuffle();
            for (u_int8_t j = 0; j < 5; j++) communityCards[j] = deck.draw();
            for (u_int8_t j = 0; j < players; j++) {
                playerCards[j].first = deck.draw();
                playerCards[j].second = deck.draw();
            }
            // simulate a showdown and remember any winners and splits as well as the total for each occurring hand
            handUtils.evaluateHands(communityCards, playerCards, players);
            // reset deck
            deck.reset();
        }
        // write the results for each player count to a file
        handUtils.writeResults(filename, players, players == 2);
        std::cout << "Wrote results for " << +players << " players\n";
    }

    PLOG_INFO << "Finished Handstrengths Tool";

    return 0;
}