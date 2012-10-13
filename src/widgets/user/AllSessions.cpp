/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WEnvironment>
#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WTableView>

#include "widgets/user/AllSessions.hpp"
#include "widgets/user/user_anchor.hpp"
#include "Application.hpp"

namespace thechess {

enum {
    SESSION_IP,
    SESSION_USER,
    SESSION_CREATED,
    SESSION_SERVER_USAGE
};

AllSessions::AllSessions() {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    boost::mutex::scoped_lock lock(Application::all_sessions_mutex());
    int row = 0;
    BOOST_FOREACH (Application* app, Application::all_sessions()) {
        Wt::WText* t;
        t = new Wt::WText(app->environment().clientAddress());
        elementAt(row, SESSION_IP)->addWidget(t);
        elementAt(row, SESSION_USER)->addWidget(user_anchor(app->user()));
        t = new Wt::WText(app->created().toString());
        elementAt(row, SESSION_CREATED)->addWidget(t);
        t = new Wt::WText(td2str(app->server_usage()));
        elementAt(row, SESSION_SERVER_USAGE)->addWidget(t);
        row += 1;
    }
    setStyleClass("thechess-table-border");
}

}
