

#include "model/CompetitionParameters.hpp"
#include "config.hpp"

namespace thechess {
namespace model {

void CompetitionParameters::initialize_()
{
    namespace defaults = config::competition::defaults;
    GameParameters::initialize_();
    type_ = STAGED;
    min_rating_ = defaults::min_rating;
    max_rating_ = defaults::max_rating;
    min_classification_ = defaults::min_classification;
    max_classification_ = defaults::max_classification;
    force_start_delay_ = defaults::force_start_delay;

    min_users_ = defaults::min_users;
    max_users_ = defaults::max_users;
    min_recruiting_time_ = defaults::min_recruiting_time;
    max_recruiting_time_ = defaults::max_recruiting_time;

    max_simultaneous_games_ = defaults::max_simultaneous_games;
    games_factor_ = defaults::games_factor;

    relax_time_ = defaults::relax_time;

    set_norating(defaults::norating);
}

CompetitionParameters* CompetitionParameters::create_new()
{
    CompetitionParameters* c = new CompetitionParameters();
    c->initialize_();
    return c;
}

}
}

