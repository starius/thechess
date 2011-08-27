/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include "model/CompetitionParameters.hpp"
#include "config.hpp"

namespace thechess {
namespace defaults = config::competition::defaults;

CompetitionParameters::CompetitionParameters() {
}

CompetitionParameters::CompetitionParameters(bool):
    GameParameters(true),
    type_(STAGED),
    min_rating_(defaults::MIN_RATING),
    max_rating_(defaults::MAX_RATING),
    min_classification_(defaults::MIN_CLASSIFICATION),
    max_classification_(defaults::MAX_CLASSIFICATION),
    force_start_delay_(defaults::FORCE_START_DELAY),

    min_users_(defaults::MIN_USERS),
    max_users_(defaults::MAX_USERS),
    min_recruiting_time_(defaults::MIN_RECRUITING_TIME),
    max_recruiting_time_(defaults::MAX_RECRUITING_TIME),

    max_simultaneous_games_(defaults::MAX_SIMULTANEOUS_GAMES),
    games_factor_(defaults::GAMES_FACTOR),

    relax_time_(defaults::RELAX_TIME),
    min_substages_(defaults::MIN_SUBSTAGES),
    increment_substages_(defaults::INCREMENT_SUBSTAGES) {
    set_norating(defaults::NORATING);
}

}

