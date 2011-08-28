/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MOVES_H_
#define THECHESS_WIDGETS_MOVES_H_

// move=-1 --> board=0 (start position) --> move=1 -->board=1 ...

#include <Wt/WContainerWidget>
#include <Wt/WSignal>
#include <Wt/WCompositeWidget>

#include "chess/Moves.hpp"
#include "chess/Piece.hpp"
#include "chess/CachedMoves.hpp"
#include "widgets/BoardWidget.hpp"

namespace thechess {

class MovesWidgetImpl;

class MovesWidget : public Wt::WCompositeWidget {
public:
    MovesWidget(const Moves& moves, bool big = false,
                bool active = false, int max_moves = -1, bool append_only = false,
                Color bottom = WHITE, Wt::WContainerWidget* parent = 0);

    const Moves& moves() const;
    const Board& board() const;
    Wt::Signal<Move>& move();
    int current_move() const;

    void add_move(const Move& move);
    void bottom_set(Color bottom);
    void reset();
    void set_moves(const Moves& moves);
    void set_active(bool active);

private:
    MovesWidgetImpl* impl_;
};

}

#endif
