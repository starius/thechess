
#include <algorithm>
#include <boost/foreach.hpp>

#include "model/Competition.hpp"
#include "config.hpp"
#include "rand.hpp"

namespace thechess {
namespace model {

Competition::Competition()
{
}

Competition::Competition(bool):
CompetitionParameters(true),
state_(RECRUITING),
created_(now())
{
}

void Competition::create_competition(UserPtr user)
{
    init_ = user;
}

void Competition::check(Objects& objects)
{
    if (state_ == RECRUITING)
    {
        if (can_start_())
        {
            start_(objects);
        }
        else if (now() - created_ > max_recruiting_time())
        {
            cancel_();
        }
    }
    if (state_ == ACTIVE && process_(objects))
    {
        finish_(objects);
    }
}

Wt::WDateTime Competition::next_check() const
{
    Wt::WDateTime result;
    if (state_ == RECRUITING)
    {
        if (type() == CLASSICAL || type() == STAGED)
        {
            if (now() < created_ + min_recruiting_time())
            {
                result = created_ + min_recruiting_time();
            }
            else
            {
                result = created_ + max_recruiting_time();
            }
        }
    }
    return result;
}

bool Competition::is_member(UserPtr user) const
{
    return members_.find().where("id = ?").bind(user).resultList().size() == 1;
}

bool Competition::is_winner(UserPtr user) const
{
    return winners_.find().where("id = ?").bind(user).resultList().size() == 1;
}

std::vector<UserPtr> Competition::members_vector() const
{
    return std::vector<UserPtr>(members_.begin(), members_.end());
}

std::vector<GamePtr> Competition::games_vector() const
{
    return std::vector<GamePtr>(games_.begin(), games_.end());
}

std::vector<UserPtr> Competition::winners_vector() const
{
    return std::vector<UserPtr>(winners_.begin(), winners_.end());
}

Games Competition::games_with(UserPtr user) const
{
    return games_.find().where("white_id = ? or black_id = ?")
        .bind(user).bind(user);
}

std::vector<GamePtr> Competition::games_with(UserPtr user,
    GamesTable& gt) const
{
    std::vector<GamePtr> result;
    BOOST_FOREACH(GamesTable::value_type& i, gt)
        BOOST_FOREACH(GamePtr game, i.second[user])
            result.push_back(game);
    BOOST_FOREACH(GamesTable::mapped_type::value_type& i, gt[user])
        BOOST_FOREACH(GamePtr game, i.second)
            result.push_back(game);
    return result;
}

GamesTable Competition::games_table() const
{
    BOOST_ASSERT(type() == CLASSICAL || type() == TEAM);
    GamesTable result;
    BOOST_FOREACH(GamePtr game, games_vector())
    {
        result[game->white()][game->black()].push_back(game);
    }
    return result;
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
        cancel_();
    }
}

void Competition::cancel_()
{
    state_ = CANCELLED;
}

bool Competition::can_start_() const
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

void Competition::start_(Objects& objects)
{
    if (type() == CLASSICAL)
    {
        create_games_classical_(objects);
    }
    else if (type() == STAGED)
    {
        create_games_staged_(objects);
    }
    else if (type() == TEAM)
    {
        // FIXME
    }
    state_ = ACTIVE;
    started_ = now();
}

void Competition::create_games_classical_(Objects& objects)
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
            create_game_(white, black);
        }
        objects.push_back(Object(UserObject, white.id()));
    }
}

void Competition::create_games_staged_(Objects& objects)
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
        create_game_(white, black, 0);
        objects.push_back(Object(UserObject, white.id()));
        objects.push_back(Object(UserObject, black.id()));
    }
}

bool Competition::process_(Objects& objects)
{
    bool finished = false;
    if (type() == CLASSICAL)
    {
        finished = process_classical_(objects);
    }
    else if (type() == STAGED)
    {
        // FIXME
    }
    else if (type() == TEAM)
    {
        // FIXME
    }
    return finished;
}

bool Competition::process_classical_(Objects& objects)
{
    BOOST_ASSERT(type() == CLASSICAL);
    std::map<UserPtr, int> used;
    std::vector<GamePtr> proposed;
    BOOST_FOREACH(GamePtr g, games_vector())
    {
        if (g->state() == Game::active || g->state() == Game::confirmed)
        {
            used[g->white()] += 1;
            used[g->black()] += 1;
        }
        if (g->state() == Game::proposed)
        {
            proposed.push_back(g);
        }
    }
    bool finished = proposed.empty();
    std::random_shuffle(proposed.begin(), proposed.end(), random::rand_for_shuffle);
    BOOST_FOREACH(GamePtr g, proposed)
    {
        if (used[g->white()] < max_simultaneous_games() &&
            used[g->black()] < max_simultaneous_games())
        {
            g.modify()->confirm_by_competition();
            objects.push_back(Object(GameObject, g.id()));
            used[g->white()] += 1;
            used[g->black()] += 1;
        }
    }
    return finished;
}

void Competition::finish_(Objects& objects)
{
    if (type() == CLASSICAL)
    {
        find_winners_classical_(objects);
    }
    if (type() == STAGED)
    {
        // FIXME
    }
    if (type() == TEAM)
    {
        // FIXME
    }
    state_ = ENDED;
    ended_ = now();
}

void Competition::find_winners_classical_(Objects&)
{
    std::map<UserPtr, int> wins;
    BOOST_FOREACH(GamePtr g, games_vector())
    {
        BOOST_ASSERT(g->is_ended());
        if (g->is_draw())
        {
            wins[g->white()] += 5;
            wins[g->black()] += 5;
        }
        else
        {
            wins[g->winner()] += 10;
        }
    }
    std::vector<UserPtr> winners;
    int max_wins;
    BOOST_FOREACH(UserPtr u, members_vector())
    {
        if (max_wins == -1 || wins[u] > max_wins)
        {
            max_wins = wins[u];
            winners.clear();
            winners.push_back(u);
        }
    }
    BOOST_FOREACH(UserPtr u, winners)
    {
        winners_.insert(u);
    }
}

GamePtr Competition::create_game_(UserPtr white, UserPtr black, int stage, bool no_draw)
{
    GamePtr game = session()->add(new Game(true));
    bool random = no_draw;
    game.modify()->make_competition_game(white, black,
        session()->load<Competition>(id()), stage, random);
    game.modify()->set_game_parameters(this);
    if (no_draw)
    {
        game.modify()->set_no_draw();
    }
    return game;
}

}
}

