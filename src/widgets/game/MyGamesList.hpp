/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MYGAMESLIST_HPP_
#define THECHESS_WIDGETS_MYGAMESLIST_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

#include "model/all.hpp"

namespace thechess {

class MyGamesListImp;

class MyGamesList : public Wt::WCompositeWidget {
public:
    MyGamesList(const UserPtr& user, Wt::WContainerWidget* p = 0);

    /** Select game */
    void select_game(const GamePtr& game);

private:
    MyGamesListImp* impl_;
};

}

#endif

