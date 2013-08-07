/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <set>
#include <algorithm>
#include <sstream>
#include <boost/foreach.hpp>

#include <Wt/Wc/rand.hpp>
#include <Wt/Wc/util.hpp>

#include "model/all.hpp"
#include "config.hpp"
#include "log.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::Competition);

namespace thechess {

Competition::Competition() {
}

Competition::Competition(bool):
    state_(RECRUITING)
{ }

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

bool Competition::can_create_competition(const UserPtr& user) {
    return user && user->has_permission(COMPETITION_CREATOR);
}

void Competition::create_competition(const UserPtr& user) {
    set_init(user);
}

void Competition::check(Wt::Wc::notify::TaskPtr task) {
    if (state_ == RECRUITING) {
        if (can_start()) {
            start();
        } else if (now() - created() > cp_->max_recruiting_time()) {
            cancel_impl();
        }
    }
    if (state_ == ACTIVE) {
        process();
    }
    t_task(task, next_check());
}

Wt::WDateTime Competition::next_check() const {
    Wt::WDateTime result;
    if (state_ == RECRUITING) {
        if (type() == CLASSICAL || type() == STAGED) {
            if (now() < created() + cp_->min_recruiting_time()) {
                result = created() + cp_->min_recruiting_time();
            } else {
                result = created() + cp_->max_recruiting_time();
            }
        }
    }
    return result;
}

void Competition::revive() {
    if (state_ == CANCELLED) {
        state_ = RECRUITING;
    }
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
    BOOST_FOREACH (const GamePtr& g, games) {
        if (!g->is_ended()) {
            return false;
        }
    }
    return true;
}

void Competition::wins_number(const GamesVector& games,
                              std::map<UserPtr, float>& wins) {
    BOOST_FOREACH (const GamePtr& g, games) {
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
        const UserPtr& u = user2float.first;
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

void Competition::set_cp(const CPPtr& cp) {
    if (cp_) {
        cp_.modify()->set_competitions_size(cp_->competitions_size() - 1);
        bool prev_team = type() == TEAM;
        bool next_team = cp->type() == TEAM;
        if (prev_team != next_team && state() == RECRUITING) {
            // remove all users
            dbo::Session& s = *self()->session();
            s.execute("delete from members_competitions where "
                      "thechess_competition_id=?").bind(id());
            s.execute("delete from thechess_tcm where "
                      "competition_id=?").bind(id());
        }
    }
    cp_ = cp;
    cp.modify()->set_competitions_size(cp->competitions_size() + 1);
}

const GPPtr& Competition::gp() const {
    const GPPtr& gp = gp_;
    return gp ? gp_ : cp()->gp();
}

CompetitionType Competition::type() const {
    return cp_->type();
}

bool Competition::is_member(const UserPtr& user) const {
    return user && members_.count(user);
}

bool Competition::is_winner(const UserPtr& user) const {
    return user && winners_.count(user);
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

Games Competition::games_with(const UserPtr& user) const {
    return games_.find().where("white_id = ? or black_id = ?")
           .bind(user).bind(user);
}

GamesVector Competition::games_with(const UserPtr& user,
                                    GamesTable& gt) const {
    GamesVector result;
    BOOST_FOREACH (GamesTable::value_type& i, gt) {
        BOOST_FOREACH (const GamePtr& game, i.second[user]) {
            result.push_back(game);
        }
    }
    BOOST_FOREACH (GamesTable::mapped_type::value_type& i, gt[user]) {
        BOOST_FOREACH (const GamePtr& game, i.second) {
            result.push_back(game);
        }
    }
    return result;
}

GamesTable Competition::games_table() const {
    BOOST_ASSERT(type() == CLASSICAL || type() == TEAM);
    GamesTable result;
    BOOST_FOREACH (const GamePtr& game, games_vector()) {
        result[game->white()][game->black()].push_back(game);
    }
    return result;
}

static void competition_virtuals_scores(const Competition* c,
                                        BD::Scores& scores) {
    std::stringstream ids_stream;
    bool first = true;
    BOOST_FOREACH (UserPtr user, c->members_vector()) {
        if (!first) {
            ids_stream << ",";
        }
        first = false;
        ids_stream << user.id();
    }
    std::string ids = ids_stream.str();
    if (ids.empty()) {
        return; // no members => no virtuals
    }
    dbo::Query<BD::BDPair> pairs = BD::pairs(*c->session());
    pairs.where("U.user_id in (" + ids + ") and V.user_id in (" + ids + ")");
    BD::scores(pairs, scores);
    BD::filter(scores);
}

bool Competition::has_virtuals() const {
    BD::Scores scores;
    competition_virtuals_scores(this, scores);
    return !scores.empty();
}

UsersVector Competition::virtuals() const {
    BD::Scores scores;
    competition_virtuals_scores(this, scores);
    std::set<UserPtr> users;
    BD::add_users(scores, users);
    return UsersVector(users.begin(), users.end());
}

void Competition::stat_change() const {
    UsersVector members = members_vector();
    UsersVector winners = winners_vector();
    std::set<UserPtr> set_of_winners;
    EloPlayers elo_winners;
    EloPlayers elo_losers;
    BOOST_FOREACH (const UserPtr& u, winners) {
        set_of_winners.insert(u);
        elo_winners.push_back(&u.modify()->competitions_stat());
    }
    BOOST_FOREACH (UserPtr u, members) {
        if (set_of_winners.find(u) == set_of_winners.end()) {
            elo_losers.push_back(&u.modify()->competitions_stat());
        }
    }
    if (!elo_winners.empty() && !elo_losers.empty()) {
        EloPlayer::multiple(elo_winners, elo_losers);
    }
}

bool Competition::can_join(const UserPtr& user) const {
    return can_join_common(user) && (type() == STAGED || type() == CLASSICAL);
}

void Competition::join(const UserPtr& user) {
    if (can_join(user)) {
        members_.insert(user);
    }
}

bool Competition::can_team_join(const UserPtr& user,
                                const TeamPtr& team) const {
    return can_join_common(user) &&
           team &&
           !team->removed() &&
           team->members().count(user);
}

void Competition::team_join(const UserPtr& user, const TeamPtr& team) {
    if (can_team_join(user, team)) {
        members_.insert(user);
        tcm_add(team, self(), user);
    }
}

bool Competition::can_leave(const UserPtr& user) const {
    return state_ == RECRUITING &&
           is_member(user);
}

void Competition::leave(const UserPtr& user) {
    if (can_leave(user)) {
        members_.erase(user);
        if (type() == TEAM) {
            tcm_remove(self(), user);
        }
    }
}

bool Competition::can_kick(const UserPtr& kicker, const UserPtr& kicked) const {
    return state_ == RECRUITING &&
           is_member(kicked) &&
           kicker &&
           kicker != kicked &&
           (kicker == init() || kicker->has_permission(COMPETITION_CHANGER));
}

void Competition::kick(const UserPtr& kicker, const UserPtr& kicked) {
    if (can_kick(kicker, kicked)) {
        members_.erase(kicked);
        if (type() == TEAM) {
            tcm_remove(self(), kicked);
        }
    }
}

bool Competition::can_change_parameters(const UserPtr& user) const {
    return (state_ == RECRUITING || state_ == CANCELLED) && user &&
           (user == init() || user->has_permission(COMPETITION_CHANGER));
}

bool Competition::can_cancel(const UserPtr& user) const {
    return state_ == RECRUITING &&
           user &&
           (user == init() || user->has_permission(COMPETITION_CHANGER));
}

void Competition::cancel(const UserPtr& user) {
    if (can_cancel(user)) {
        cancel_impl();
    }
}

bool Competition::can_force_start(const UserPtr& user) const {
    namespace ccm = config::competition::min;
    return state_ == RECRUITING &&
           user &&
           (user == init() || user->has_permission(COMPETITION_CHANGER)) &&
           int(members_.size()) >= ccm::MIN_USERS &&
           now() - created() >= ccm::MIN_RECRUITING_TIME &&
           (virtual_allower_ || !has_virtuals());
}

void Competition::force_start(const UserPtr& user) {
    if (can_force_start(user)) {
        start();
        t_task(COMPETITION, id());
    }
}

void Competition::cancel_impl() {
    state_ = CANCELLED;
    ended_ = now();
}

bool Competition::can_allow_virtuals(const UserPtr& user) const {
    return state_ == RECRUITING && user && !virtual_allower() &&
           user->has_permission(COMPETITION_CHANGER) &&
           user->has_permission(VIRTUALS_VIEWER);
}

void Competition::allow_virtuals(const UserPtr& user) {
    if (can_allow_virtuals(user)) {
        virtual_allower_ = user;
    }
}

bool Competition::has_comment_base() const {
    return comment_base_;
}

const CommentPtr& Competition::comment_base() {
    if (!comment_base_) {
        comment_base_ = session()->add(new Comment(true));
        comment_base_.modify()->set_type(Comment::CHAT_ROOT);
        comment_base_.flush();
    }
    return comment_base_;
}

bool Competition::can_add_team(const UserPtr& user, const TeamPtr& team) const {
    return state_ == RECRUITING &&
           user &&
           !teams_.count(team) &&
           !team->removed() &&
           team->init() == user;
}

void Competition::add_team(const UserPtr& user, const TeamPtr& team) {
    if (can_add_team(user, team)) {
        teams_.insert(team);
    }
}

bool Competition::can_remove_team(const UserPtr& user,
                                  const TeamPtr& team) const {
    return state_ == RECRUITING &&
           user &&
           teams_.count(team) &&
           (team->init() == user || init() == user ||
            user->has_permission(COMPETITION_CHANGER));
}

void Competition::remove_team(const UserPtr& user, const TeamPtr& team) {
    if (can_remove_team(user, team)) {
        teams_.erase(team);
    }
}

bool Competition::can_start() const {
    bool result = false;
    if (state_ == RECRUITING) {
        if (type() == CLASSICAL || type() == STAGED) {
            if (static_cast<int>(members_.size()) >= cp_->min_users() &&
                    now() - created() >= cp_->min_recruiting_time()) {
                result = true;
                if (!virtual_allower_ && has_virtuals()) {
                    result = false;
                    admin_log("Can't start competition " + TO_S(id()) +
                              " because of virtuals",
                              *session(), UserPtr(), true);
                }
            }
        }
    }
    return result;
}

void Competition::start() {
    if (type() == CLASSICAL) {
        create_games_classical();
    }
    state_ = ACTIVE;
    started_ = now();
}

void Competition::create_games_classical() {
    UsersVector members(members_.begin(), members_.end());
    std::random_shuffle(members.begin(), members.end(),
                        Wt::Wc::rand_for_shuffle);
    int white_games_per_user_raw = (members.size() - 1) * cp_->games_factor();
    int white_games_per_user = std::max(1, white_games_per_user_raw);
    std::map<UserPtr, int> black_games;
    std::map<UserPtr, std::map<UserPtr, int> > N; // games number between them
    BOOST_FOREACH (const UserPtr& white, members) {
        std::map<UserPtr, int>& N_white = N[white];
        for (int i = 0; i < white_games_per_user; i++) {
            UserPtr black;
            BOOST_FOREACH (const UserPtr& user, members) {
                if (user != white &&
                        black_games[user] != white_games_per_user) {
                    bool good_black_user = false;
                    // compare number of games:
                    // N_white[user] : where white_id=white and black_id=user
                    // N_white[black] : where white_id=white and black_id=black
                    if (!black || N_white[user] < N_white[black]) {
                        good_black_user = true;
                    }
                    if (N_white[user] == N_white[black]) {
                        // compare total number of games
                        int white_vs_user = N[white][user] + N[user][white];
                        int white_vs_black = N[white][black] + N[black][white];
                        if (white_vs_user < white_vs_black) {
                            good_black_user = true;
                        }
                    }
                    if (good_black_user) {
                        black = user;
                    }
                }
            }
            BOOST_ASSERT(black);
            black_games[black] += 1;
            N[white][black] += 1;
            create_game(white, black);
        }
        t_emit_after(USER, white.id());
    }
}

void Competition::process() {
    if (type() == CLASSICAL) {
        process_classical();
        GamesVector g(games_vector());
        if (all_ended(g)) {
            UsersVector winners = winners_of_games(g);
            finish(winners);
        }
    } else if (type() == STAGED) {
        StagedCompetition sc(this);
        sc.process(this);
        if (sc.winner()) {
            UsersVector winners;
            winners.push_back(sc.winner());
            finish(winners);
        }
    }
}

void Competition::process_classical() {
    BOOST_ASSERT(type() == CLASSICAL);
    std::map<UserPtr, int> used;
    GamesVector proposed;
    BOOST_FOREACH (const GamePtr& g, games_vector()) {
        if (g->state() == Game::PROPOSED && !g->created().isValid()) {
            proposed.push_back(g);
        } else if (!g->is_ended()) {
            used[g->white()] += 1;
            used[g->black()] += 1;
        }
    }
    std::random_shuffle(proposed.begin(), proposed.end(),
                        Wt::Wc::rand_for_shuffle);
    BOOST_FOREACH (const GamePtr& g, proposed) {
        if (used[g->white()] < cp_->max_simultaneous_games() &&
                used[g->black()] < cp_->max_simultaneous_games()) {
            g.modify()->propose_by_competition();
            t_task(GAME, g.id());
            used[g->white()] += 1;
            used[g->black()] += 1;
        }
    }
}

void Competition::finish(const UsersVector& winners) {
    UsersVector members = members_vector();
    BOOST_FOREACH (const UserPtr& u, winners) {
        winners_.insert(u);
    }
    state_ = ENDED;
    ended_ = now();
    stat_change();
}

GamePtr Competition::create_game(const UserPtr& white, const UserPtr& black,
                                 int stage, bool no_draw) {
    GPPtr gp_ptr = gp();
    if (no_draw) {
        GPPtr no_draw;
        dbo::Query<GPPtr> q = session()->find<GP>()
                              .where("first_draw = ?").bind(NO_DRAW)
                              .where("parent_id = ?").bind(gp_ptr);
        if (q.resultList().size()) {
            no_draw = q.resultList().front();
        } else {
            no_draw = session()->add(new GP(true));
            no_draw.modify()->apply_parameters(*gp_ptr);
            no_draw.modify()->set_parent(gp_ptr);
            no_draw.modify()->set_no_draw();
        }
        gp_ptr = no_draw;
    }
    GamePtr game = session()->add(new Game(gp_ptr));
    bool random = no_draw;
    game.modify()->make_competition_game(white, black, self(), stage, random);
    game.flush();
    return game;
}

bool Competition::can_join_common(const UserPtr& user) const {
    return state_ == RECRUITING &&
           user && !is_member(user) &&
           user->has_permission(COMPETITION_JOINER) &&
           user->games_stat().elo() >= cp_->min_rating() &&
           user->games_stat().elo() <= cp_->max_rating() &&
           user->classification() >= cp_->min_classification() &&
           user->classification() <= cp_->max_classification() &&
           user->online_time() >= cp_->min_online_time() &&
           (user->online_time() <= cp_->max_online_time() ||
            cp_->max_online_time() < SECOND) &&
           static_cast<int>(members_.size()) < cp_->max_users();
}

}

