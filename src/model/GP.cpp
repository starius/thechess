/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <algorithm>

#include "model/all.hpp"
#include "Options.hpp"
#include "config.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::GP);

namespace thechess {

GP::GP() {
}

GP::GP(bool):
    moves_(Moves()),
    limit_std_(config::defaults::LIMIT_STD),
    limit_private_init_(config::defaults::LIMIT_PRIVATE_INIT),
    pause_limit_init_(config::defaults::PAUSE_LIMIT_INIT),
    norating_(config::defaults::NORATING),
    first_draw_(config::defaults::FIRST_DRAW),
    games_size_(0) {
}

int GP::first_draw() const {
    int min_first_draw = Options::instance()->min_first_draw();
    return std::max(first_draw_, min_first_draw);
}

void GP::set_no_draw() {
    using namespace config; // min
    first_draw_ = NO_DRAW;
    limit_std_ = min::LIMIT_STD;
    limit_private_init_ = min::LIMIT_PRIVATE_INIT;
    pause_limit_init_ = min::PAUSE_LIMIT_INIT;
}

}

