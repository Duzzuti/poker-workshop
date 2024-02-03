#include "test_player.h"

Action TestPlayer::turn(const Data&, const bool) const noexcept {
    if (this->actionIndex == 0) {
        std::cout << "TestPlayer took to many actions. GO AND FIX YOUR TEST" << std::endl;
        return Action{Actions::FOLD, 0};
    }
    return this->actions[--this->actionIndex];
}