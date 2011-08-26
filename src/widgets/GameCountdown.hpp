/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_
#define THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_

#include <Wt/WViewWidget>

#include "model/Game.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {

class GameCountdown : public Wt::WViewWidget {
public:
    GameCountdown(GamePtr game, Wt::WContainerWidget* parent = 0);
private:
    GamePtr game_;

    virtual Wt::WWidget* renderView();
};

}

#endif
