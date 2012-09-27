/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMELISTWIDGET_HPP_
#define THECHESS_WIDGETS_GAMELISTWIDGET_HPP_

#include <vector>

#include <Wt/WContainerWidget>

#include "chess/global.hpp"
#include "model/global.hpp"

namespace thechess {

class GameListWidget : public Wt::WContainerWidget {
public:
    GameListWidget(Wt::WContainerWidget* parent = 0);

    GameListWidget(const UserPtr& user, Wt::WContainerWidget* parent = 0);

    GameListWidget(const CompetitionPtr& c, Wt::WContainerWidget* parent = 0);

    GameListWidget(const std::vector<int>& games,
                   Wt::WContainerWidget* parent = 0);

    GameListWidget(const Board& board, Wt::WContainerWidget* parent = 0);

    GameListWidget(const Moves& moves, Wt::WContainerWidget* parent = 0);
};

}

#endif

