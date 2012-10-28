/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MOVES_H_
#define THECHESS_WIDGETS_MOVES_H_

// half_move=-1 --> board=0 (start position) --> half_move=1 -->board=1 ...

#include <Wt/WContainerWidget>
#include <Wt/WSignal>
#include <Wt/WCompositeWidget>

#include "chess/Moves.hpp"
#include "chess/Piece.hpp"
#include "chess/CachedMoves.hpp"
#include "widgets/chess/BoardWidget.hpp"

namespace thechess {

class MovesWidget : public Wt::WCompositeWidget {
public:
    typedef BoardWidget::LinksHandler LinksHandler;

    MovesWidget(const Moves& moves, bool big = false, bool active = false,
                int max_moves = -1, bool append_only = false,
                Piece::Color bottom = Piece::WHITE,
                Wt::WContainerWidget* parent = 0);

    const Moves& moves() const;
    const Board& board() const;
    Wt::Signal<HalfMove>& half_move();
    int current_move() const;

    /* Show half-move with given index */
    void set_half_move(int half_move_index);

    void add_move(const HalfMove& half_move);
    void bottom_set(Piece::Color bottom);
    void reset();
    void set_moves(const Moves& moves);
    void set_active(bool active);

    /** Set if confirmation checkbox is added under the BoardWidget.
    Defaults to false.
    */
    void set_move_confirmation(bool needed = true);

    /** Set links handler.
    \see BoardWidget::set_links_handler
    */
    void set_links_handler(const LinksHandler& links_handler);

    /** Add MovesWidget specific things to LinksDialog */
    void links(LinksDialog* dialog);

private:
    class MovesWidgetImpl;

    MovesWidgetImpl* impl_;
};

}

#endif

