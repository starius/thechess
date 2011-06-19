#include "model/GameParameters.hpp"
#include "config.hpp"

namespace thechess {
namespace model {
using namespace config; // defaults, min

GameParameters::GameParameters()
{
}

GameParameters::GameParameters(bool):
moves_(chess::Moves()),
moves_init_(0),
limit_std_(defaults::limit_std),
limit_private_init_(defaults::limit_private_init),
pause_limit_init_(defaults::pause_limit_init),
norating_(defaults::norating),
first_draw_(defaults::first_draw)
{
}

void GameParameters::set_game_parameters(const GameParameters* other)
{
    *this = *other;
}

void GameParameters::set_no_draw()
{
    first_draw_ = NO_DRAW;
    limit_std_ = min::limit_std;
    limit_private_init_ = min::limit_private_init;
    pause_limit_init_ = min::pause_limit_init;
}

}
}

