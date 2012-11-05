/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_TEAM_LIST_WIDGET_HPP_
#define THECHESS_WIDGETS_TEAM_LIST_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

namespace thechess {

/** List of teams */
class TeamList : public Wt::WContainerWidget {
public:
    /** Constructor */
    TeamList();

private:
    void team_create();
};

}

#endif

