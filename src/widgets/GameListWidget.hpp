/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMELISTWIDGET_HPP_
#define THECHESS_WIDGETS_GAMELISTWIDGET_HPP_

#include <Wt/WCompositeWidget>

namespace Wt {
    class WContainerWidget;
}

namespace thechess {
namespace widgets {

class GameListWidgetImpl;

class GameListWidget : public Wt::WCompositeWidget
{
public:
    GameListWidget(Wt::WContainerWidget* parent=0);
private:
    GameListWidgetImpl* impl_;
};

}
}

#endif
