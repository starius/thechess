/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMEWIDGET_HPP_
#define THECHESS_WIDGETS_GAMEWIDGET_HPP_

#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include "model/Game.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {

class GameWidgetImpl;

class GameWidget : public Wt::WCompositeWidget {
public:
    GameWidget(GamePtr game, Wt::WContainerWidget* parent = 0);
private:
    GameWidgetImpl* impl_;
};

}

#endif
