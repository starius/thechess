#include "model/GameParameters.hpp"
#include "config.hpp"

namespace thechess {
namespace model {
using namespace config; // defaults

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

}
}

