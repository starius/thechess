/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_TEAM_WIDGET_HPP_
#define THECHESS_WIDGETS_TEAM_WIDGET_HPP_

#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

class TeamWidget : public Wt::WContainerWidget, public Notifiable {
public:
    TeamWidget(const TeamPtr& team);

    void notify(EventPtr);

private:
    TeamPtr team_;

    void reprint();
};

}

#endif

