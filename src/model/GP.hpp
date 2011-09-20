/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_GAMAPARAMATERS_HPP_
#define THECHESS_MODEL_GAMAPARAMATERS_HPP_

#include "model/global.hpp"
#include "chess/Moves.hpp"
#include "chess/Piece.hpp"

namespace thechess {

class GP : public dbo::Dbo<GP> {
public:
    GP();
    GP(bool);

    template<class Action>
    void persist(Action& a) {
        dbo::hasMany(a, games_, dbo::ManyToOne, "gp");
        dbo::hasMany(a, cps_, dbo::ManyToOne, "cp");
        dbo::field(a, moves_.as_svuc(), "moves");
        dbo::field(a, limit_std_, "limit_std");
        dbo::field(a, limit_private_init_, "limit_private_init");
        dbo::field(a, norating_, "norating");
        dbo::field(a, pause_limit_init_, "pause_limit_init");
        dbo::field(a, first_draw_, "first_draw");
    }

    const Moves& moves() const {
        return moves_;
    }

    void set_moves(const Moves& moves) {
        moves_ = moves;
    }

    const Td& limit_std() const {
        return limit_std_;
    }
    void set_limit_std(const Td& limit_std) {
        limit_std_ = limit_std;
    }

    const Td& limit_private_init() const {
        return limit_private_init_;
    }
    void set_limit_private_init(const Td& limit_private_init) {
        limit_private_init_ = limit_private_init;
    }

    bool norating() const {
        return norating_;
    }
    void set_norating(bool norating) {
        norating_ = norating;
    }

    const Td& pause_limit_init() const {
        return pause_limit_init_;
    }
    void set_pause_limit_init(const Td& pause_limit_init) {
        pause_limit_init_ = pause_limit_init;
    }

    int first_draw() const {
        return first_draw_;
    }
    void set_first_draw(int first_draw) {
        first_draw_ = first_draw;
    }

    void set_gp(const GP* other);
    void set_no_draw();

private:
    Games games_;
    CPs cps_;

    Moves moves_;

    Td limit_std_;
    Td limit_private_init_;
    Td pause_limit_init_;

    bool norating_;

    int first_draw_;
};

const int NO_DRAW = -1;

}

#endif
