/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMELISTWIDGET_HPP_
#define THECHESS_WIDGETS_GAMELISTWIDGET_HPP_

#include <Wt/WContainerWidget>

#include "model/global.hpp"

namespace thechess {

class GameListWidget : public Wt::WContainerWidget {
public:
    GameListWidget(Wt::WContainerWidget* parent = 0);

    GameListWidget(const UserPtr& user, Wt::WContainerWidget* parent = 0);
};

}

#endif

