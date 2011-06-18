#ifndef THECHESS_MODEL_GAMAPARAMATERS_HPP_
#define THECHESS_MODEL_GAMAPARAMATERS_HPP_

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

namespace thechess {
namespace model {
    class GameParameters;
}
}

#include "chess/moves.hpp"
#include "chess/field.hpp"
#include "time_intervals.hpp"

namespace thechess {
namespace model {

class GameParameters
{
public:
    GameParameters();
    GameParameters(bool);

    template<class Action>
    void persist(Action& a)
    {
        dbo::field(a, moves_.as_svuc(), "moves");
        dbo::field(a, moves_init_, "moves_init");
        dbo::field(a, limit_std_, "limit_std");
        dbo::field(a, limit_private_init_, "limit_private_init_");
        dbo::field(a, norating_, "norating");
        dbo::field(a, pause_limit_init_, "pause_limit_init");
        dbo::field(a, first_draw_, "first_draw");
    }

    const chess::Moves& moves() const { return moves_; }

    void set_init_moves(const chess::Moves& moves)
    {
        moves_ = moves;
        moves_init_ = moves_.size();
    }

    int moves_init() const { return moves_init_; }

    const Td& limit_std() const { return limit_std_; }
    void set_limit_std(const Td& limit_std) { limit_std_ = limit_std; }

    const Td& limit_private_init() const { return limit_private_init_; }
    void set_limit_private_init(const Td& limit_private_init)
    {
        limit_private_init_ = limit_private_init;
    }

    bool norating() const { return norating_; }
    void set_norating(bool norating) { norating_ = norating; }

    const Td& pause_limit_init() const { return pause_limit_init_; }
    void set_pause_limit_init(const Td& pause_limit_init)
    {
        pause_limit_init_ = pause_limit_init;
    }

    int first_draw() const { return first_draw_; }
    void set_first_draw(int first_draw)
    {
        first_draw_ = first_draw;
    }

protected:
    chess::Moves moves_;

private:
    int moves_init_;

    Td limit_std_;
    Td limit_private_init_;
    Td pause_limit_init_;

    bool norating_;

    int first_draw_;
};

}
}

#endif
