
#include <algorithm>
#include <boost/foreach.hpp>

#include "model/Competition.hpp"
#include "config.hpp"
#include "rand.hpp"

namespace thechess {
namespace model {

void Competition::initialize_()
{
    CompetitionParameters::initialize_();
}

Competition* Competition::create_new()
{
    Competition* c = new Competition();
    c->initialize_();
    return c;
}

void Competition::create_competition(UserPtr user)
{
    state_ = RECRUITING;
    created_ = now();
    init_ = user;
}

void Competition::check(Objects& objects)
{

}

Wt::WDateTime Competition::next_check() const
{
    return now() + minute; // FIXME
}

bool Competition::is_member(UserPtr user) const
{
    return members_.find().where("id = ?").bind(user).resultList().size() == 1;
}

bool Competition::can_join(UserPtr user) const
{
    return state_ == RECRUITING &&
        !is_member(user) &&
        user->elo_rating() >= min_rating() &&
        user->elo_rating() <= max_rating() &&
        user->classification() >= min_classification() &&
        user->classification() <= max_classification();
}

void Competition::join(UserPtr user)
{
    if (can_join(user))
    {
        members_.insert(user);
    }
}

bool Competition::can_leave(UserPtr user) const
{
    return state_ == RECRUITING &&
        is_member(user);
}

void Competition::leave(UserPtr user)
{
    if (can_leave(user))
    {
        members_.erase(user);
    }
}

bool Competition::can_kick(UserPtr kicker, UserPtr kicked) const
{
    return state_ == RECRUITING &&
        is_member(kicked) &&
        kicker == init_;
}

void Competition::kick(UserPtr kicker, UserPtr kicked)
{
    if (can_kick(kicker, kicked))
    {
        kick(kicker, kicked);
    }
}

bool Competition::can_change_parameters(UserPtr user) const
{
    return state_ == RECRUITING &&
        user == init_;
}

bool Competition::can_cancel(UserPtr user) const
{
    return state_ == RECRUITING &&
        user == init_;
}

void Competition::cancel(UserPtr user)
{
    if (can_cancel(user))
    {
        state_ = CANCELLED;
    }
}

bool Competition::is_winner(UserPtr user) const
{
    return winners_.find().where("id = ?").bind(user).resultList().size() == 1;
}

bool Competition::can_start() const
{
    bool result = false;
    if (state_ == RECRUITING)
    {
        if (type() == CLASSICAL || type() == STAGED)
        {
            if (static_cast<int>(members_.size()) >= min_users() &&
                now() - created_ >= min_recruiting_time())
            {
                result = true;
            }
        }
    }
    return result;
}

void Competition::start()
{
    if (can_start())
    {
        if (type() == CLASSICAL)
        {
            create_games_classical_();
        }
        else if (type() == STAGED)
        {
            create_games_staged_();
        }
        else if (type() == TEAM)
        {
        }
        state_ = ACTIVE;
        started_ = now();
    }
}

void Competition::create_games_classical_()
{
    std::vector<UserPtr> members(members_.begin(), members_.end());
    std::random_shuffle(members.begin(), members.end(), random::rand_for_shuffle);
    int white_games_per_user = std::max(1, int((members.size()-1) * games_factor()));
    std::map<UserPtr, int> black_games;
    std::map<UserPtr, std::map<UserPtr, int> > N; // number of all games between them
    BOOST_FOREACH(UserPtr white, members)
    {
        std::map<UserPtr, int>& N_white = N[white];
        for (int i = 0; i < white_games_per_user; i++)
        {
            UserPtr black;
            BOOST_FOREACH(UserPtr user, members)
            {
                if (user != white && black_games[user] != white_games_per_user)
                    if (!black || N_white[user] < N_white[black])
                        black = user;
            }
            BOOST_ASSERT(black);
            black_games[black] += 1;
            N[white][black] += 1;
            N[black][white] += 1;
            GamePtr game = session()->add(Game::create_new());
            game.modify()->make_competition_game(white, black,
                session()->load<Competition>(id()));
        }
    }
}

void Competition::create_games_staged_()
{
    std::vector<UserPtr> members(members_.begin(), members_.end());
    std::random_shuffle(members.begin(), members.end(), random::rand_for_shuffle);
    int members_size = members.size();
    int max_pow2 = pow(2, floor(log2(members_size))) + 0.5;
    int pairs = members_size - max_pow2 || members_size / 2;
    for (int i = 0; i < pairs; i++)
    {
        UserPtr white = members[2*i];
        UserPtr black = members[2*i+1];
        GamePtr game = session()->add(Game::create_new());
        game.modify()->make_competition_game(white, black,
            session()->load<Competition>(id()), 0);
    }
}

}
}

