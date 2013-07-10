/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/all.hpp"
#include "config.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::CP);

namespace thechess {
namespace ccd = config::competition::defaults;

CP::CP() {
}

CP::CP(bool):
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
    increment_substages_(ccd::INCREMENT_SUBSTAGES),
    min_online_time_(ccd::MIN_ONLINE_TIME),
    max_online_time_(ccd::MAX_ONLINE_TIME),
    competitions_size_(0)
{ }

void CP::apply_parameters(const CP& other) {
    parent_ = other.parent_;
    gp_ = other.gp_;
    type_ = other.type_;
    min_rating_ = other.min_rating_;
    max_rating_ = other.max_rating_;
    min_classification_ = other.min_classification_;
    max_classification_ = other.max_classification_;
    force_start_delay_ = other.force_start_delay_;
    min_users_ = other.min_users_;
    max_users_ = other.max_users_;
    min_recruiting_time_ = other.min_recruiting_time_;
    max_recruiting_time_ = other.max_recruiting_time_;
    max_simultaneous_games_ = other.max_simultaneous_games_;
    games_factor_ = other.games_factor_;
    relax_time_ = other.relax_time_;
    min_substages_ = other.min_substages_;
    increment_substages_ = other.increment_substages_;
    min_online_time_ = other.min_online_time_;
    max_online_time_ = other.max_online_time_;
    competitions_size_ = other.competitions_size_;
}

}

