#ifndef THECHESS_MODEL_COMPETITION_HPP_
#define THECHESS_MODEL_COMPETITION_HPP_

#include <string>
#include <map>

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
}
}

#include "model/CompetitionParameters.hpp"
#include "model/User.hpp"
#include "model/Game.hpp"
#include "model/td.hpp"
#include "model/Object.hpp"

namespace thechess {
namespace model {

typedef std::map<UserPtr, std::map<UserPtr, std::vector<GamePtr> > > GamesTable;

class Competition : public CompetitionParameters, public dbo::Dbo<Competition>
{
public:
    typedef CompetitionType Type;

    enum State
    {
        RECRUITING = 10,
        ACTIVE = 30,
        ENDED = 50,
        CANCELLED = 70
    };

    Competition();
    Competition(bool);

    static Wt::WString type2str(Type type);
    static bool all_ended(const std::vector<GamePtr>& games);
    static std::vector<UserPtr> winners_of_games(const std::vector<GamePtr>& games);

    template<class Action>
    void persist(Action& a)
    {
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

    State state() const { return state_; }

    const Wt::WString& name() const { return name_; }
    void set_name(Wt::WString& v) { name_ = v; }
    const Wt::WString& description() const { return description_; }
    void set_description(Wt::WString& v) { description_ = v; }

    bool is_member(UserPtr user) const;
    bool is_winner(UserPtr user) const;

    const Users& members() const { return members_; }
    const Games& games() const { return games_; }
    const Users& winners() const { return winners_; }

    std::vector<UserPtr> members_vector() const;
    std::vector<GamePtr> games_vector() const;
    std::vector<UserPtr> winners_vector() const;

    GamesTable games_table() const;
    Games games_with(UserPtr user) const;
    std::vector<GamePtr> games_with(UserPtr user, GamesTable& gt) const;

    // auto-manage
    void check(Objects& objects);
    Wt::WDateTime next_check() const;

    // methods of recruiting state
    static bool can_create_competition(UserPtr /*user*/) { return true; }
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
    void cancel_();

    void create_games_classical_(Objects& objects);
    void create_games_staged_(Objects& objects);
    void create_games_team_(Objects& objects);

    bool process_(Objects& objects);
    bool process_classical_(Objects& objects);
    bool process_staged_(Objects& objects);
    bool process_team_(Objects& objects);

    void finish_(Objects& objects);
    void find_winners_classical_(Objects& objects);
    void find_winners_staged_(Objects& objects);

    GamePtr create_game_(UserPtr white, UserPtr black, int stage=-1,
        bool no_draw=false);
};

}
}

#endif

