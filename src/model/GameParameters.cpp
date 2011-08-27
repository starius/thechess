/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "model/GameParameters.hpp"
#include "config.hpp"

namespace thechess {
using namespace config; // defaults, min

GameParameters::GameParameters() {
}

GameParameters::GameParameters(bool):
    moves_(Moves()),
    moves_init_(0),
    limit_std_(defaults::LIMIT_STD),
    limit_private_init_(defaults::LIMIT_PRIVATE_INIT),
    pause_limit_init_(defaults::PAUSE_LIMIT_INIT),
    norating_(defaults::NORATING),
    first_draw_(defaults::FIRST_DRAW) {
}

void GameParameters::set_game_parameters(const GameParameters* other) {
    *this = *other;
}

void GameParameters::set_no_draw() {
    first_draw_ = NO_DRAW;
    limit_std_ = min::LIMIT_STD;
    limit_private_init_ = min::LIMIT_PRIVATE_INIT;
    pause_limit_init_ = min::PAUSE_LIMIT_INIT;
}

}

