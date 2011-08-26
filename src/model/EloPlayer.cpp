/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <cmath>
#include <boost/foreach.hpp>

#include "model/EloPlayer.hpp"
#include "config.hpp"

namespace thechess {
namespace model {
namespace elo = config::elo;

const float victory = 1.0;
const float fail = 0.0;

EloPlayer::EloPlayer() {
}

EloPlayer::EloPlayer(bool):
    elo_(config::elo::init),
    all_(0),
    wins_(0),
    fails_(0) {
}

int EloPlayer::draws() const {
    return all_ - wins_ - fails_;
}

float EloPlayer::Q() const {
    return pow(10.0, float(elo_) / 400);
}

float EloPlayer::E(float q_sum) const {
    return Q() / q_sum;
}

float EloPlayer::E(const EloPlayer* other) const {
    return E(Q() + other->Q());
}

float EloPlayer::K() const {
    return (elo_ >= elo::big) ? elo::big_K :
           (all_ < elo::beginner_all) ? elo::beginner_K : elo::other_K;
}

void EloPlayer::apply_result_(float q_sum, float S) {
    elo_ += round(K() * (S - E(q_sum)));
    all_ += 1;
    if (S == victory) {
        wins_ += 1;
    } else if (S == fail) {
        fails_ += 1;
    }
}

void EloPlayer::win(EloPlayer* loser) {
    float q_sum = Q() + loser->Q();
    this->apply_result_(q_sum, victory);
    loser->apply_result_(q_sum, fail);
}

void EloPlayer::draw(EloPlayer* other) {
    float q_sum = Q() + other->Q();
    this->apply_result_(q_sum, 0.5);
    other->apply_result_(q_sum, 0.5);
}

void EloPlayer::multiple(const EloPlayers& winners, const EloPlayers& losers) {
    float q_sum = 0;
    BOOST_FOREACH (EloPlayer* player, winners) {
        q_sum += player->Q();
    }
    BOOST_FOREACH (EloPlayer* player, losers) {
        q_sum += player->Q();
    }
    BOOST_FOREACH (EloPlayer* player, winners) {
        player->apply_result_(q_sum, victory / winners.size());
    }
    BOOST_FOREACH (EloPlayer* player, losers) {
        player->apply_result_(q_sum, fail);
    }
}

}
}

