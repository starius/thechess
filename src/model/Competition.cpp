/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <algorithm>
#include <boost/foreach.hpp>

#include "model/Competition.hpp"
#include "model/StagedCompetition.hpp"
#include "config.hpp"
#include "rand.hpp"

namespace thechess {

Competition::Competition() {
}

Competition::Competition(bool):
    CompetitionParameters(true),
    state_(RECRUITING),
    created_(now()) {
}

Wt::WString Competition::type2str(Type type) {
    if (type == CLASSICAL) {
        return Wt::WString::tr("tc.competition.Type_classical");
    }
    if (type == STAGED) {
        return Wt::WString::tr("tc.competition.Type_staged");
    }
    if (type == TEAM) {
        return Wt::WString::tr("tc.competition.Type_team");
    }
    return Wt::WString::tr("tc.competition.Type");
}

void Competition::create_competition(UserPtr user) {
    init_ = user;
}

void Competition::check(Objects& objects) {
    if (state_ == RECRUITING) {
        if (can_start_()) {
            start_(objects);
        } else if (now() - created_ > max_recruiting_time()) {
            cancel_();
        }
    }
    if (state_ == ACTIVE) {
        process_(objects);
    }
}

Wt::WDateTime Competition::next_check() const {
    Wt::WDateTime result;
    if (state_ == RECRUITING) {
        if (type() == CLASSICAL || type() == STAGED) {
            if (now() < created_ + min_recruiting_time()) {
                result = created_ + min_recruiting_time();
            } else {
                result = created_ + max_recruiting_time();
            }
        }
    }
    return result;
}

const char* Competition::state2str(State state) {
    if (state == RECRUITING) {
        return "tc.competition.state_recruiting";
    }
    if (state == ACTIVE) {
        return "tc.competition.state_active";
    }
    if (state == ENDED) {
        return "tc.competition.state_ended";
    }
    if (state == CANCELLED) {
        return "tc.competition.state_cancelled";
    }
    return "tc.game.State_state";
}

bool Competition::all_ended(const GamesVector& games) {
    BOOST_FOREACH (GamePtr g, games) {
        if (!g->is_ended()) {
            return false;
        }
    }
    return true;
}

void Competition::wins_number(const GamesVector& games,
                              std::map<UserPtr, float>& wins) {
    BOOST_FOREACH (GamePtr g, games) {
        if (g->is_draw()) {
            wins[g->white()] += 0.5;
            wins[g->black()] += 0.5;
        } else {
            wins[g->winner()] += 1;
        }
    }
}

UsersVector Competition::winners_of_games(const GamesVector& games) {
    typedef std::map<UserPtr, float> User2float;
    User2float wins;
    wins_number(games, wins);
    UsersVector winners;
    float max_wins = -1;
    BOOST_FOREACH (User2float::value_type& user2float, wins) {
        UserPtr u = user2float.first;
        float w = user2float.second;
        if (max_wins < 0 || w - max_wins > 0.1) {
            max_wins = w;
            winners.clear();
        }
        if (abs(w - max_wins) < 0.1) {
            winners.push_back(u);
        }
    }
    return winners;
}

bool Competition::is_member(UserPtr user) const {
    return user && members_.find().where("id = ?").bind(user.id()).resultList().size() == 1;
}

bool Competition::is_winner(UserPtr user) const {
    return user && winners_.find().where("id = ?").bind(user.id()).resultList().size() == 1;
}

UsersVector Competition::members_vector() const {
    return UsersVector(members_.begin(), members_.end());
}

GamesVector Competition::games_vector() const {
    return GamesVector(games_.begin(), games_.end());
}

UsersVector Competition::winners_vector() const {
    return UsersVector(winners_.begin(), winners_.end());
}

Games Competition::games_with(UserPtr user) const {
    return games_.find().where("white_id = ? or black_id = ?")
           .bind(user.id()).bind(user.id());
}

GamesVector Competition::games_with(UserPtr user,
                                    GamesTable& gt) const {
    GamesVector result;
    BOOST_FOREACH (GamesTable::value_type& i, gt) {
        BOOST_FOREACH (GamePtr game, i.second[user]) {
            result.push_back(game);
        }
    }
    BOOST_FOREACH (GamesTable::mapped_type::value_type& i, gt[user]) {
        BOOST_FOREACH (GamePtr game, i.second) {
            result.push_back(game);
        }
    }
    return result;
}

GamesTable Competition::games_table() const {
    BOOST_ASSERT(type() == CLASSICAL || type() == TEAM);
    GamesTable result;
    BOOST_FOREACH (GamePtr game, games_vector()) {
        result[game->white()][game->black()].push_back(game);
    }
    return result;
}

bool Competition::can_join(UserPtr user) const {
    return state_ == RECRUITING &&
           user && !is_member(user) &&
           user->games_stat().elo() >= min_rating() &&
           user->games_stat().elo() <= max_rating() &&
           user->classification() >= min_classification() &&
           user->classification() <= max_classification() &&
           static_cast<int>(members_.size()) < max_users();
}

void Competition::join(UserPtr user) {
    if (can_join(user)) {
        members_.insert(user);
    }
}

bool Competition::can_leave(UserPtr user) const {
    return state_ == RECRUITING &&
           is_member(user);
}

void Competition::leave(UserPtr user) {
    if (can_leave(user)) {
        members_.erase(user);
    }
}

bool Competition::can_kick(UserPtr kicker, UserPtr kicked) const {
    return state_ == RECRUITING &&
           is_member(kicked) &&
           kicker == init_;
}

void Competition::kick(UserPtr kicker, UserPtr kicked) {
    if (can_kick(kicker, kicked)) {
        members_.erase(kicked);
    }
}

bool Competition::can_change_parameters(UserPtr user) const {
    return state_ == RECRUITING && user &&
           (user == init_ || user->rights() >= User::moderator);
}

bool Competition::can_cancel(UserPtr user) const {
    return state_ == RECRUITING &&
           user == init_;
}

void Competition::cancel(UserPtr user) {
    if (can_cancel(user)) {
        cancel_();
    }
}

void Competition::cancel_() {
    state_ = CANCELLED;
}

bool Competition::can_start_() const {
    bool result = false;
    if (state_ == RECRUITING) {
        if (type() == CLASSICAL || type() == STAGED) {
            if (static_cast<int>(members_.size()) >= min_users() &&
                    now() - created_ >= min_recruiting_time()) {
                result = true;
            }
        }
    }
    return result;
}

void Competition::start_(Objects& objects) {
    if (type() == CLASSICAL) {
        create_games_classical_(objects);
    }
    state_ = ACTIVE;
    started_ = now();
}

void Competition::create_games_classical_(Objects& objects) {
    UsersVector members(members_.begin(), members_.end());
    std::random_shuffle(members.begin(), members.end(), rand_for_shuffle);
    int white_games_per_user = std::max(1, int((members.size() - 1) * games_factor()));
    std::map<UserPtr, int> black_games;
    std::map<UserPtr, std::map<UserPtr, int> > N; // number of all games between them
    BOOST_FOREACH (UserPtr white, members) {
        std::map<UserPtr, int>& N_white = N[white];
        for (int i = 0; i < white_games_per_user; i++) {
            UserPtr black;
            BOOST_FOREACH (UserPtr user, members) {
                if (user != white && black_games[user] != white_games_per_user)
                    if (!black || N_white[user] < N_white[black]) {
                        black = user;
                    }
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

void Competition::process_(Objects& objects) {
    if (type() == CLASSICAL) {
        process_classical_(objects);
        GamesVector g(games_vector());
        if (all_ended(g)) {
            UsersVector winners = winners_of_games(g);
            finish_(winners, objects);
        }
    } else if (type() == STAGED) {
        StagedCompetition sc(this);
        sc.process(this, objects);
        if (sc.winner()) {
            UsersVector winners;
            winners.push_back(sc.winner());
            finish_(winners, objects);
        }
    }
}

void Competition::process_classical_(Objects& objects) {
    BOOST_ASSERT(type() == CLASSICAL);
    std::map<UserPtr, int> used;
    GamesVector proposed;
    BOOST_FOREACH (GamePtr g, games_vector()) {
        if (g->state() == Game::active || g->state() == Game::confirmed) {
            used[g->white()] += 1;
            used[g->black()] += 1;
        }
        if (g->state() == Game::proposed) {
            proposed.push_back(g);
        }
    }
    std::random_shuffle(proposed.begin(), proposed.end(), rand_for_shuffle);
    BOOST_FOREACH (GamePtr g, proposed) {
        if (used[g->white()] < max_simultaneous_games() &&
                used[g->black()] < max_simultaneous_games()) {
            g.modify()->confirm_by_competition();
            objects.push_back(Object(GameObject, g.id()));
            used[g->white()] += 1;
            used[g->black()] += 1;
        }
    }
}

void Competition::finish_(const UsersVector& winners, Objects&) {
    BOOST_FOREACH (UserPtr u, winners) {
        winners_.insert(u);
    }
    state_ = ENDED;
    ended_ = now();
}

GamePtr Competition::create_game_(UserPtr white, UserPtr black, int stage, bool no_draw) {
    GamePtr game = session()->add(new Game(true));
    bool random = no_draw;
    game.modify()->make_competition_game(white, black,
                                         session()->load<Competition>(id()), stage, random);
    game.modify()->set_game_parameters(this);
    if (no_draw) {
        game.modify()->set_no_draw();
    }
    return game;
}

}

