#include "hand_utils.h"
#include "mainargs.h"
#include "working_dir.h"

int main(const int argc, const char** argv) {
    srand(time(NULL));  // init random seed

    WorkingDir workingDir{argv[0], "log_tool.txt"};
    MainArgs mainArgs(argc, argv);
    mainArgs.appendDescriptionLine("Handstrengths Data Tool");
    mainArgs.appendDescriptionLine("This tool is used to simulate a bunch of showdowns and extract the handstrengths data");
    mainArgs.appendDescriptionLine("The data is then written to a csv file in the data directory");
    mainArgs.setFlag('v', "verbose", "Enable verbose logging");
    mainArgs.setFlag('s', "silent", "Disable logging");
    mainArgs.setNumericArg('i', "iters", 1, INT64_MAX, "The number of iterations to simulate");
    mainArgs.setStringArg('o', "output",
                          "The name of the output file in the data directory (with extension) \n\t\t(default: 'STR_TOOL_HANDSTRENGTHS_DEFAULT_FILE_NAME + options + .csv')\n\t\tNote that the correct "
                          "file options are required (-w)");
    mainArgs.setNumericArg('w', "woptions", 1, 999, "An 3 digit number to set the weight of the winner, split and total count");
    if (!mainArgs.run()) return 1;

    // init logger
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    // add file logger
    static plog::RollingFileAppender<plog::TxtFormatter> fileAppender(workingDir.getLogPath().c_str(), 1024 * 1024 * 10, 5);

    // default options
    u_int8_t winnerAdd = DEFAULT_HANDSTRENGTHS_ANALYTICS_OPTIONS[0];
    u_int8_t splitAdd = DEFAULT_HANDSTRENGTHS_ANALYTICS_OPTIONS[1];
    u_int8_t totalAdd = DEFAULT_HANDSTRENGTHS_ANALYTICS_OPTIONS[2];
    int64_t iters = DEFAULT_HANDSTRENGTHS_ANALYTICS_ITERS;
    std::string filename = "";

    // verbose logging
    if (mainArgs.isArgSet(0)) {
        if (mainArgs.isArgSet(1)) {
            std::cerr << "Cannot set both verbose and silent logging arguments" << std::endl;
            return 1;
        }
        plog::init(plog::verbose, &consoleAppender).addAppender(&fileAppender);
    }
    // silent logging
    else if (!mainArgs.isArgSet(1)) {
        // default to info level
        plog::init(plog::info, &consoleAppender).addAppender(&fileAppender);
    }
    PLOG_INFO << "Starting Handstrengths Tool";

    // iterations
    if (mainArgs.isArgSet(2)) iters = mainArgs.getArgNumericValue(2);

    // filename
    if (mainArgs.isArgSet(3)) {
        PLOG_INFO << "Output file: " << mainArgs.getArgValue(3) << std::endl;
        if (!mainArgs.isArgSet(4)) {
            std::cerr << "Custom output filename requires options (-w)" << std::endl;
            return 1;
        }
        filename = mainArgs.getArgValue(3);
    }
    // options
    if (mainArgs.isArgSet(4)) {
        winnerAdd = mainArgs.getArgNumericValue(4) / 100;
        splitAdd = mainArgs.getArgNumericValue(4) / 10 % 10;
        totalAdd = mainArgs.getArgNumericValue(4) % 10;
        if (totalAdd < std::max(winnerAdd, splitAdd)) {
            std::cerr << "The totalAdd value has to be greater or equal than the maximum of winnerAdd and splitAdd" << std::endl;
            return 1;
        }
        if (totalAdd == 0) {
            std::cerr << "The totalAdd value has to be greater than 0" << std::endl;
            return 1;
        }
    }
    // default filename
    if (filename == "") {
        filename = STR_TOOL_HANDSTRENGTHS_DEFAULT_FILE_NAME + std::to_string(+winnerAdd) + std::to_string(+splitAdd) + std::to_string(+totalAdd) + ".csv";
    }

    Deck deck;
    Card communityCards[5];
    std::pair<Card, Card> playerCards[MAX_PLAYERS];
    // iterate over any meaningful number of players
    for (u_int8_t players = 2; players <= MAX_PLAYERS; players++) {
        // set up HandUtils
        HandUtils handUtils(winnerAdd, splitAdd, totalAdd);
        // simulate for an amount of iterations
        for (int64_t i = 0; i < iters; i++) {
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
        handUtils.writeResults((workingDir.getDataPath() / filename).string(), players, players == 2);
        std::cout << "Wrote results for " << +players << " players\n";
    }

    PLOG_INFO << "Finished Handstrengths Tool";

    return 0;
}