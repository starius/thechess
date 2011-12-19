/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_
#define THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_

#include <Wt/WViewWidget>

#include "model/all.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {

class GameCountdown : public Wt::WViewWidget {
public:
    GameCountdown(const GamePtr& game, Wt::WContainerWidget* parent = 0);
private:
    GamePtr game_;

    virtual Wt::WWidget* renderView();
};

}

#endif

