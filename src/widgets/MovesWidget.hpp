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

#include "chess/moves.hpp"
#include "chess/field.hpp"
#include "chess/cached_moves.hpp"
#include "widgets/BoardWidget.hpp"

namespace thechess {
namespace widgets {

class MovesWidgetImpl;

class MovesWidget : public Wt::WCompositeWidget
{
public:
    MovesWidget(const chess::Moves& moves, bool big=false,
        bool active=false, int max_moves=-1, bool append_only=false,
        chess::Color bottom=chess::white, Wt::WContainerWidget* parent=0);

    const chess::Moves& moves() const;
    const chess::Board& board() const;
    Wt::Signal<chess::Move>& move();
    int current_move() const;

    void add_move(const chess::Move& move);
    void bottom_set(chess::Color bottom);
    void reset();
    void set_moves(const chess::Moves& moves);
    void set_active(bool active);

private:
    MovesWidgetImpl* impl_;
};

}
}

#endif // THECHESS_WIDGETS_MOVES_H_
