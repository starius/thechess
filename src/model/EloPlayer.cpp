
#include <cmath>
#include <boost/foreach.hpp>

#include "model/EloPlayer.hpp"

namespace thechess {
namespace model {

namespace elo = config::elo;

const float victory = 1.0;
const float fail = 0.0;

int EloPlayer::number_of_draws() const
{
    return number_of_games_ - number_of_wins_ - number_of_fails_;
}

float EloPlayer::Q() const
{
    return pow(10.0, float(elo_rating_) / 400);
}

float EloPlayer::E(float q_sum) const
{
    return Q() / q_sum;
}

float EloPlayer::E(const EloPlayer* other) const
{
    return E(Q() + other->Q());
}

float EloPlayer::K() const
{
    return (elo_rating_ >= elo::big_rating) ? elo::big_K : (
        (number_of_games_ < elo::beginner_number_of_games) ? 
        elo::beginner_K : elo::other_K);
}

void EloPlayer::apply_result_(float E, float S)
{
    elo_rating_ += round(K() * (S - E));
    number_of_games_ += 1;
    if (S == victory)
    {
        number_of_wins_ += 1;
    }
    else if (S == fail)
    {
        number_of_fails_ += 1;
    }
}

void EloPlayer::win(EloPlayer* loser)
{
    this->apply_result_(this->E(loser), victory);
    loser->apply_result_(loser->E(this), fail);
}

void EloPlayer::win(const std::list<EloPlayer*>& losers)
{
    float q_sum = 0;
    BOOST_FOREACH(EloPlayer* loser, losers)
    {
        q_sum += loser->Q();
    }
    BOOST_FOREACH(EloPlayer* loser, losers)
    {
        loser->apply_result_(loser->E(q_sum), fail);
    }
    this->apply_result_(this->E(q_sum), victory);
}

void EloPlayer::draw(EloPlayer* a, EloPlayer* b)
{
    a->apply_result_(a->E(b), 0.5);
    b->apply_result_(b->E(a), 0.5);
}

void EloPlayer::draw(const std::list<EloPlayer*>& players)
{
    float q_sum = 0;
    BOOST_FOREACH(EloPlayer* player, players)
    {
        q_sum += player->Q();
    }
    BOOST_FOREACH(EloPlayer* player, players)
    {
        player->apply_result_(player->E(q_sum), 1.0 / players.size());
    }
}

}
}
