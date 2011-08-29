/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

// vim: set noet ts=2 sw=2:
/* Board widget
 * Signal: half_move()
 */

#ifndef THECHESS_WIDGETS_BOARDWIDGET_H_
#define THECHESS_WIDGETS_BOARDWIDGET_H_

#include <Wt/WCompositeWidget>
#include <Wt/WImage>

#include "chess/Board.hpp"
#include "chess/Square.hpp"
#include "chess/HalfMove.hpp"
#include "chess/Piece.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {

class BoardWidgetImpl;

class BoardWidget : public Wt::WCompositeWidget {
public:
    BoardWidget(bool big = false, bool active = false, Piece::Color bottom = Piece::WHITE,
                const Board& board = Board(),
                Wt::WContainerWidget* parent = 0);

    static std::string image(Piece piece, bool big = false);

    void bottom_set(Piece::Color bottom);
    void set(const Board& board, HalfMove lastmove, bool active);

    void turn();
    Wt::Signal<HalfMove>& half_move();
    Wt::WContainerWidget* inner();

private:
    BoardWidgetImpl* impl_;
};

}

#endif

