#ifndef THECHESS_MODEL_ELOPLAYER_HPP_
#define THECHESS_MODEL_ELOPLAYER_HPP_

#include <list>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

namespace thechess {
namespace model {
    class EloPlayer;
}
}

#include "config.hpp"

namespace thechess {
namespace model {

class EloPlayer
{
public:
    EloPlayer() :
    elo_rating_(config::elo::init_rating),
    number_of_games_(0),
    number_of_wins_(0),
    number_of_fails_(0)
    {
    }

    template<class Action>
    void persist(Action& a)
    {
        dbo::field(a, elo_rating_, "elo_rating");
        dbo::field(a, number_of_games_, "number_of_games");
        dbo::field(a, number_of_wins_, "number_of_wins");
        dbo::field(a, number_of_fails_, "number_of_fails");
    }

    int elo_rating() const { return elo_rating_; }
    int number_of_games() const { return number_of_games_; }
    int number_of_wins() const { return number_of_wins_; }
    int number_of_fails() const { return number_of_fails_; }
    int number_of_draws() const;

    float K() const;
    float Q() const;
    float E(const EloPlayer* other) const;
    float E(float q_sum) const;

    void win(EloPlayer* loser);
    void win(const std::list<EloPlayer*>& losers);
    static void draw(EloPlayer* a, EloPlayer* b);
    static void draw(const std::list<EloPlayer*>& players);

private:
    int elo_rating_;
    int number_of_games_;
    int number_of_wins_;
    int number_of_fails_;

    void apply_result_(float E, float S);
};

}
}

#endif

