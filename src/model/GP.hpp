/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_GAMAPARAMATERS_HPP_
#define THECHESS_MODEL_GAMAPARAMATERS_HPP_

#include "model/model.hpp"
#include "model/record.hpp"
#include "chess/Moves.hpp"
#include "chess/Piece.hpp"

namespace thechess {

/** Parameters of a game.
This model stores abstract game parameters, which could be used
by multiple games and \ref CP "competition parameters".

\ingroup model
*/
class GP : public dbo::Dbo<GP>, public Record {
public:
    /** Constructor for Wt::Dbo internal usage */
    GP();

    /** Create instance to be added to database */
    GP(bool);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        Record::persist(a);
        dbo::belongsTo(a, parent_, "parent");
        dbo::hasMany(a, children_, dbo::ManyToOne, "parent");
        dbo::hasMany(a, games_, dbo::ManyToOne, "gp");
        dbo::hasMany(a, cps_, dbo::ManyToOne, "gp");
        dbo::hasMany(a, competitions_, dbo::ManyToOne, "gp");
        dbo::field(a, moves_, "moves");
        dbo::field(a, limit_std_, "limit_std");
        dbo::field(a, limit_private_init_, "limit_private_init");
        dbo::field(a, norating_, "norating");
        dbo::field(a, pause_limit_init_, "pause_limit_init");
        dbo::field(a, first_draw_, "first_draw");
        dbo::field(a, games_size_, "games_size");
    }

    /** Get init moves */
    const Moves& moves() const {
        return moves_;
    }

    /** Set init moves */
    void set_moves(const Moves& moves) {
        moves_ = moves;
    }

    /** Get renewable time limit */
    const Td& limit_std() const {
        return limit_std_;
    }

    /** Set renewable time limit */
    void set_limit_std(const Td& limit_std) {
        limit_std_ = limit_std;
    }

    /** Get exhaustible time limit */
    const Td& limit_private_init() const {
        return limit_private_init_;
    }

    /** Set exhaustible time limit */
    void set_limit_private_init(const Td& limit_private_init) {
        limit_private_init_ = limit_private_init;
    }

    /** Get if games produced by this game parameters could affect rating */
    bool norating() const {
        return norating_;
    }

    /** Set if games produced by this game parameters could affect rating */
    void set_norating(bool norating) {
        norating_ = norating;
    }

    /** Get pause limit */
    const Td& pause_limit_init() const {
        return pause_limit_init_;
    }

    /** Set pause limit */
    void set_pause_limit(const Td& pause_limit_init) {
        pause_limit_init_ = pause_limit_init;
    }

    /** Get first half-move allowed to be draw or surrender */
    int first_draw() const {
        return first_draw_;
    }

    /** Set first half-move allowed to be draw or surrender */
    void set_first_draw(int first_draw) {
        first_draw_ = first_draw;
    }

    /** Get parent */
    const GPPtr& parent() const {
        return parent_;
    }

    /** Set parent */
    void set_parent(const GPPtr& parent) {
        parent_ = parent;
    }

    /** Return list of GPs based on this GP */
    const GPs& children() const {
        return children_;
    }

    /** Return list of Games based on this GP */
    const Games& games() const {
        return games_;
    }

    /** Return list of CPs based on this GP */
    const CPs& cps() const {
        return cps_;
    }

    /** Return list of Competitions based on this GP */
    const Competitions& competitions() const {
        return competitions_;
    }

    /** Set min time limits and forbid game being finished with draw.
    All types of draw become impossible except stalemate.
    In case of stalemate games is ended with NO_DRAW_STALEMATE, black wins.
    */
    void set_no_draw();

    /** Return cached number of games */
    int games_size() const {
        return games_size_;
    }

    /** Set cached number of games */
    void set_games_size(int games_size) {
        games_size_ = games_size;
    }

private:
    GPPtr parent_;
    GPs children_;

    Games games_;
    CPs cps_;
    Competitions competitions_;

    Moves moves_;

    Td limit_std_;
    Td limit_private_init_;
    Td pause_limit_init_;

    bool norating_;

    int first_draw_;
    int games_size_;
};

/** This value of first_draw() indicates a game without draw possibility */
const int NO_DRAW = -1;

}

DBO_EXTERN_TEMPLATES(thechess::GP);

#endif

