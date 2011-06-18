#ifndef THECHESS_MODEL_COMPETITION_PARAMETERS_HPP_
#define THECHESS_MODEL_COMPETITION_PARAMETERS_HPP_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

namespace thechess {
namespace model {
    class CompetitionParameters;
}
}

#include "model/GameParameters.hpp"
#include "model/UserClassification.hpp"
#include "model/CompetitionType.hpp"
#include "model/td.hpp"

namespace thechess {
namespace model {

class CompetitionParameters : public GameParameters
{
public:
    typedef CompetitionType Type;

    CompetitionParameters();
    CompetitionParameters(bool);

    template<class Action>
    void persist(Action& a)
    {
        GameParameters::persist(a);
        dbo::field(a, type_, "type");
        dbo::field(a, min_rating_, "min_rating");
        dbo::field(a, max_rating_, "max_rating");
        dbo::field(a, min_classification_, "min_classification");
        dbo::field(a, max_classification_, "max_classification");
        dbo::field(a, force_start_delay_, "force_start_delay");
        dbo::field(a, min_users_, "min_users");
        dbo::field(a, max_users_, "max_users");
        dbo::field(a, min_recruiting_time_, "min_recruiting_time");
        dbo::field(a, max_recruiting_time_, "max_recruiting_time");
        dbo::field(a, max_simultaneous_games_, "max_simultaneous_games");
        dbo::field(a, games_factor_, "games_factor");
        dbo::field(a, relax_time_, "relax_time");
    }

    Type type() const { return type_; }
    void set_type(Type v) { type_ = v; }

    int min_rating() const { return min_rating_; }
    void set_min_rating(int v) { min_rating_ = v; }
    int max_rating() const { return max_rating_; }
    void set_max_rating(int v) { max_rating_ = v; }

    Classification min_classification() const { return min_classification_; }
    void set_min_classification(Classification v) { min_classification_ = v; }
    Classification max_classification() const { return max_classification_; }
    void set_max_classification(Classification v) { max_classification_ = v; }

    Td force_start_delay() const { return force_start_delay_; }
    void set_force_start_delay(Td v) { force_start_delay_ = v; }

    int min_users() const { return min_users_; }
    void set_min_users(int v) { min_users_ = v; }
    int max_users() const { return max_users_; }
    void set_max_users(int v) { max_users_ = v; }

    Td min_recruiting_time() const { return min_recruiting_time_; }
    void set_min_recruiting_time(Td v) { min_recruiting_time_ = v; }
    Td max_recruiting_time() const { return max_recruiting_time_; }
    void set_max_recruiting_time(Td v) { max_recruiting_time_ = v; }

    int max_simultaneous_games() const { return max_simultaneous_games_; }
    void set_max_simultaneous_games(int v) { max_simultaneous_games_ = v; }

    float games_factor() const { return games_factor_; }
    void set_games_factor(float v) { games_factor_ = v; }

    Td relax_time() const { return relax_time_; }
    void set_relax_time(Td v) { relax_time_ = v; }

private:
    Type type_;

    int min_rating_;
    int max_rating_;
    Classification min_classification_;
    Classification max_classification_;

    Td force_start_delay_;

    // classical and staged
    int min_users_;
    int max_users_;
    Td min_recruiting_time_;
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

