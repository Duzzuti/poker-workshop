#include <random>

#include "player.h"

class RandPlayer : public Player {
   public:
    RandPlayer(const std::string& name) noexcept : Player(name){};
    RandPlayer(u_int8_t playerNum = 0) noexcept : Player(!playerNum ? "RandPlayer" : "RandPlayer" + std::to_string(playerNum)){};

    Action turn(const Data& data) const noexcept override;

   private:
    mutable std::random_device dev;
    mutable std::mt19937 rng{dev()};
    mutable std::uniform_int_distribution<std::mt19937::result_type> dist{0, INT32_MAX};  // distribution in max u_int32 range
};