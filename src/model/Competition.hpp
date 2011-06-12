#ifndef THECHESS_MODEL_COMPETITION_HPP_
#define THECHESS_MODEL_COMPETITION_HPP_

#include <string>

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

namespace thechess {
namespace model {

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

    static Competition* create_new();

    static Wt::WString type2str(Type type);

    template<class Action>
    void persist(Action& a)
    {
        CompetitionParameters::persist(a);
        dbo::field(a, state_, "state");
        dbo::field(a, name_, "name");
        dbo::field(a, description_, "description");
        dbo::hasMany(a, users_, dbo::ManyToMany, "users_competitions");
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

    // auto-manage
    void check();

    // manage competition
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

protected:
    void initialize_();

private:
    // common attributes
    State state_;

    Wt::WString name_;
    Wt::WString description_;

    Users users_;
    UserPtr init_;

    Users virtuals_;
    UserPtr virtual_allower_;

    Games games_;

    Wt::WDateTime created_;
    Wt::WDateTime started_;
    Wt::WDateTime ended_;

    Users winners_;
};

}
}

#endif

