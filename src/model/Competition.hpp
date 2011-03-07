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

#include "model/GameParameters.hpp"
#include "model/User.hpp"
#include "model/Game.hpp"
#include "model/td.hpp"

namespace thechess {
namespace model {

class Competition : public GameParameters, public dbo::Dbo<Competition>
{
public:

    enum State
    {
        RECRUITING = 10,
        ACTIVE = 30,
        ENDED = 50
    };

    enum Type
    {
        CLASSICAL = 10,
        STAGED = 20,
        TEAM = 30
    };

    template<class Action>
    void persist(Action& a)
    {
        GameParameters::persist(a);
        dbo::field(a, state_, "state");
        dbo::field(a, type_, "type");
        dbo::field(a, name_, "name");
        dbo::field(a, description_, "description");
        dbo::hasMany(a, users_, dbo::ManyToMany, "users");
        dbo::belongsTo(a, init_, "init");
        dbo::field(a, min_rating_, "min_rating");
        dbo::field(a, max_rating_, "max_rating");
        dbo::field(a, min_classification_, "min_classification");
        dbo::field(a, max_classification_, "max_classification");
        dbo::hasMany(a, virtuals_, dbo::ManyToMany, "virtuals");
        dbo::belongsTo(a, virtual_allower_, "virtual_allower");
        dbo::hasMany(a, games_, dbo::ManyToOne, "competition");
        dbo::field(a, created_, "created");
        dbo::field(a, started_, "started");
        dbo::field(a, ended_, "ended");

        dbo::hasMany(a, winners_, dbo::ManyToMany, "winners");
        dbo::field(a, max_users_, "max_users");
        dbo::field(a, max_recruiting_time_, "max_recruiting_time");

        dbo::field(a, max_simultaneous_games_, "max_simultaneous_games");
        dbo::field(a, games_factor_, "games_factor");

        dbo::field(a, relax_time_, "relax_time");
    }

    void initialize();

    State state() const { return state_; }

    Type type() const { return type_; }
    void set_type(Type v) { type_ = v; }

    const Wt::WString& name() const { return name_; }
    void set_name(Wt::WString& v) { name_ = v; }

    const Wt::WString& description() const { return description_; }
    void set_description(Wt::WString& v) { description_ = v; }

    int min_rating() const { return min_rating_; }
    void set_min_rating(int v) { min_rating_ = v; }
    int max_rating() const { return max_rating_; }
    void set_max_rating(int v) { max_rating_ = v; }

    Classification min_classification() const { return min_classification_; }
    void set_min_classification(Classification v) { min_classification_ = v; }
    Classification max_classification() const { return max_classification_; }
    void set_max_classification(Classification v) { max_classification_ = v; }

    int max_users() const { return max_users_; }
    void set_max_users(int v) { max_users_ = v; }

    Td max_recruiting_time() const { return max_recruiting_time_; }
    void set_max_recruiting_time(Td v) { max_recruiting_time_ = v; }

    int max_simultaneous_games() const { return max_simultaneous_games_; }
    void set_max_simultaneous_games(int v) { max_simultaneous_games_ = v; }

    float games_factor() const { return games_factor_; }
    void set_games_factor(float v) { games_factor_ = v; }

    Td relax_time() const { return relax_time_; }
    void set_relax_time(Td v) { relax_time_ = v; }

private:

    // common attributes
    State state_;
    Type type_;

    Wt::WString name_;
    Wt::WString description_;

    Users users_;
    UserPtr init_;

    int min_rating_;
    int max_rating_;
    Classification min_classification_;
    Classification max_classification_;

    Users virtuals_;
    UserPtr virtual_allower_;

    Games games_;

    Wt::WDateTime created_;
    Wt::WDateTime started_;
    Wt::WDateTime ended_;

    // classical and staged
    Users winners_;
    int max_users_;
    Td max_recruiting_time_;

    // classical
    int max_simultaneous_games_;
    float games_factor_;

    // staged
    Td relax_time_;
};

}
}

#endif
