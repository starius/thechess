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
 * Signal: move()
 */

#ifndef THECHESS_WIDGETS_BOARDWIDGET_H_
#define THECHESS_WIDGETS_BOARDWIDGET_H_

#include <Wt/WCompositeWidget>
#include <Wt/WImage>

#include "chess/board.hpp"
#include "chess/xy.hpp"
#include "chess/move.hpp"
#include "chess/field.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {

class BoardWidgetImpl;

class BoardWidget : public Wt::WCompositeWidget {
public:
    BoardWidget(bool big = false, bool active = false, Color bottom = WHITE,
                const Board& board = Board(),
                Wt::WContainerWidget* parent = 0);

    static std::string image(Field field, bool big = false);

    void bottom_set(Color bottom);
    void set(const Board& board, Move lastmove, bool active);

    void turn();
    Wt::Signal<Move>& move();
    Wt::WContainerWidget* inner();

private:
    BoardWidgetImpl* impl_;
};

}

#endif
