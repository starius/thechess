/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/team/TeamWidget.hpp"

namespace thechess {

TeamWidget::TeamWidget(const TeamPtr& team):
    Notifiable(Object(TEAM_OBJECT, team.id())),
    team_(team) {
    dbo::Transaction t(tApp->session());
    reprint();
}

void TeamWidget::notify(EventPtr) {
    dbo::Transaction t(tApp->session());
    team_.reread();
    reprint();
}

void TeamWidget::reprint() {
    // TODO
}

}

