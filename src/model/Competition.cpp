
#include "model/Competition.hpp"
#include "config.hpp"

namespace thechess {
namespace model {


void Competition::initialize()
{
    type_ = STAGED;
    state_ = RECRUITING;
    created_ = now();
    namespace defaults = config::defaults::competition;
    min_rating_ = defaults::min_rating;
    max_rating_ = defaults::max_rating;
    min_classification_ = defaults::min_classification;
    max_classification_ = defaults::max_classification;

    max_users_ = defaults::max_users;
    max_recruiting_time_ = defaults::max_recruiting_time;

    max_simultaneous_games_ = defaults::max_simultaneous_games;
    games_factor_ = defaults::games_factor;

    relax_time_ = defaults::relax_time;
}

}
}

