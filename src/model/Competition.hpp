/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMPETITION_HPP_
#define THECHESS_MODEL_COMPETITION_HPP_

#include <string>
#include <map>
#include <vector>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include <Wt/WDateTime>
#include <Wt/Dbo/WtSqlTraits>

namespace thechess {
namespace model {
class Competition;
typedef dbo::ptr<Competition> CompetitionPtr;
typedef dbo::collection<CompetitionPtr> Competitions;
typedef std::vector<CompetitionPtr> CompetitionsVector;
}
}

#include "model/CompetitionParameters.hpp"
#include "model/User.hpp"
#include "model/Game.hpp"
#include "model/td.hpp"
#include "model/Object.hpp"

namespace thechess {
namespace model {

typedef std::map<UserPtr, std::map<UserPtr, GamesVector> > GamesTable;

class Competition : public CompetitionParameters, public dbo::Dbo<Competition> {
public:
    typedef CompetitionType Type;

    enum State {
        RECRUITING = 10,
        ACTIVE = 30,
        ENDED = 50,
        CANCELLED = 70
    };

    Competition();
    Competition(bool);

    static Wt::WString type2str(Type type);
    static bool all_ended(const GamesVector& games);
    static void wins_number(const GamesVector& games,
                            std::map<UserPtr, float>& wins);
    static UsersVector winners_of_games(const GamesVector& games);

    template<class Action>
    void persist(Action& a) {
        CompetitionParameters::persist(a);
        dbo::field(a, state_, "state");
        dbo::field(a, name_, "name");
        dbo::field(a, description_, "description");
        dbo::hasMany(a, members_, dbo::ManyToMany, "members_competitions");
        dbo::belongsTo(a, init_, "init_competitions");
        dbo::belongsTo(a, virtual_allower_, "virtual_allower");
        dbo::hasMany(a, games_, dbo::ManyToOne, "competition");
        dbo::field(a, created_, "created");
        dbo::field(a, started_, "started");
        dbo::field(a, ended_, "ended");
        dbo::hasMany(a, winners_, dbo::ManyToMany, "winners_competition");
    }

    State state() const {
        return state_;
    }
    static const char* state2str(State state);
    UserPtr init() const {
        return init_;
    }

    const Wt::WDateTime& created() const {
        return created_;
    }
    const Wt::WDateTime& started() const {
        return started_;
    }
    const Wt::WDateTime& ended() const {
        return ended_;
    }

    const Wt::WString& name() const {
        return name_;
    }
    void set_name(const Wt::WString& v) {
        name_ = v;
    }
    const Wt::WString& description() const {
        return description_;
    }
    void set_description(const Wt::WString& v) {
        description_ = v;
    }

    bool is_member(UserPtr user) const;
    bool is_winner(UserPtr user) const;

    const Users& members() const {
        return members_;
    }
    const Games& games() const {
        return games_;
    }
    const Users& winners() const {
        return winners_;
    }

    UsersVector members_vector() const;
    GamesVector games_vector() const;
    UsersVector winners_vector() const;

    GamesTable games_table() const;
    Games games_with(UserPtr user) const;
    GamesVector games_with(UserPtr user, GamesTable& gt) const;

    // auto-manage
    void check(Objects& objects);
    Wt::WDateTime next_check() const;

    // methods of recruiting state
    static bool can_create_competition(UserPtr /*user*/) {
        return true;
    }
    void create_competition(UserPtr user);

    bool can_join(UserPtr user) const;
    void join(UserPtr user);

    bool can_leave(UserPtr user) const;
    void leave(UserPtr user);

    bool can_kick(UserPtr kicker, UserPtr kicked) const;
    void kick(UserPtr kicker, UserPtr kicked);

    bool can_change_parameters(UserPtr user) const;

    bool can_cancel(UserPtr user) const;
    void cancel(UserPtr user);

    // methods of active stage

private:
    // common attributes
    State state_;

    Wt::WString name_;
    Wt::WString description_;

    Users members_;
    UserPtr init_;

    Users virtuals_;
    UserPtr virtual_allower_;

    Games games_;

    Wt::WDateTime created_;
    Wt::WDateTime started_;
    Wt::WDateTime ended_;

    Users winners_;

    bool can_start_() const;
    void start_(Objects& objects);
    void create_games_classical_(Objects& objects);
    void cancel_();

    void process_(Objects& objects);
    void process_classical_(Objects& objects);

    void finish_(const UsersVector& winners, Objects& objects);

    GamePtr create_game_(UserPtr white, UserPtr black, int stage=-1,
                         bool no_draw=false);

    friend class StagedCompetition;
};

}
}

#endif

