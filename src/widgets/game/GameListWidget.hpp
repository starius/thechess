/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMELISTWIDGET_HPP_
#define THECHESS_WIDGETS_GAMELISTWIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

namespace thechess {

class GameListWidget : public Wt::WCompositeWidget {
public:
    GameListWidget(Wt::WContainerWidget* parent = 0);
private:
    class GameListWidgetImpl;

    GameListWidgetImpl* impl_;
};

}

#endif
