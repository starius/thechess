/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMEWIDGET_HPP_
#define THECHESS_WIDGETS_GAMEWIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>

#include "model/all.hpp"

namespace thechess {

class GameWidget : public Wt::WCompositeWidget {
public:
    GameWidget(const GamePtr& game, Wt::WContainerWidget* parent = 0);

    /* Show half-move with given index */
    void set_half_move(int half_move_index);

private:
    class GameWidgetImpl;

    GameWidgetImpl* impl_;
};

}

#endif

