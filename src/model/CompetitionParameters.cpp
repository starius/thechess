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
namespace model {
namespace defaults = config::competition::defaults;

CompetitionParameters::CompetitionParameters()
{
}

CompetitionParameters::CompetitionParameters(bool):
GameParameters(true),
type_(STAGED),
min_rating_(defaults::min_rating),
max_rating_(defaults::max_rating),
min_classification_(defaults::min_classification),
max_classification_(defaults::max_classification),
force_start_delay_(defaults::force_start_delay),

min_users_(defaults::min_users),
max_users_(defaults::max_users),
min_recruiting_time_(defaults::min_recruiting_time),
max_recruiting_time_(defaults::max_recruiting_time),

max_simultaneous_games_(defaults::max_simultaneous_games),
games_factor_(defaults::games_factor),

relax_time_(defaults::relax_time),
min_substages_(defaults::min_substages),
increment_substages_(defaults::increment_substages)
{
    set_norating(defaults::norating);
}

}
}

