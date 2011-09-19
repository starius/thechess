/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "model/CP.hpp"
#include "config.hpp"

namespace thechess {
namespace ccd = config::competition::defaults;

CompetitionParameters::CompetitionParameters() {
}

CompetitionParameters::CompetitionParameters(bool):
    GameParameters(true),
    type_(STAGED),
    min_rating_(ccd::MIN_RATING),
    max_rating_(ccd::MAX_RATING),
    min_classification_(ccd::MIN_CLASSIFICATION),
    max_classification_(ccd::MAX_CLASSIFICATION),
    force_start_delay_(ccd::FORCE_START_DELAY),

    min_users_(ccd::MIN_USERS),
    max_users_(ccd::MAX_USERS),
    min_recruiting_time_(ccd::MIN_RECRUITING_TIME),
    max_recruiting_time_(ccd::MAX_RECRUITING_TIME),

    max_simultaneous_games_(ccd::MAX_SIMULTANEOUS_GAMES),
    games_factor_(ccd::GAMES_FACTOR),

    relax_time_(ccd::RELAX_TIME),
    min_substages_(ccd::MIN_SUBSTAGES),
    increment_substages_(ccd::INCREMENT_SUBSTAGES) {
    set_norating(ccd::NORATING);
}

}

