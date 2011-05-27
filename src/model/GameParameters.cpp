#include "model/GameParameters.hpp"
#include "config.hpp"

namespace thechess {
namespace model {

void GameParameters::initialize_()
{
    using namespace config; // defaults
    moves_ = chess::Moves();
    moves_init_ = 0;
    limit_std_ = defaults::limit_std;
    limit_private_init_ = defaults::limit_private_init;
    pause_limit_init_ = defaults::pause_limit_init;
    norating_ = false;
    first_draw_ = defaults::first_draw;
}

GameParameters* GameParameters::create_new()
{
    GameParameters* gp = new GameParameters();
    gp->initialize_();
    return gp;
}

}
}

