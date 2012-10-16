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
    SESSION_USER,
    SESSION_CREATED,
    SESSION_SERVER_USAGE,
    SESSION_IP,
    SESSION_COOKIE,
    SESSION_AGENT
};

AllSessions::AllSessions() {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    boost::mutex::scoped_lock lock(Application::all_sessions_mutex());
    int row = 0;
    BOOST_FOREACH (Application* app, Application::all_sessions()) {
        const std::string& ip = app->environment().clientAddress();
        Wt::WAnchor* a = new Wt::WAnchor;
        a->setText(ip);
        a->setLink(tApp->path().banned_ip()->get_link(ip));
        elementAt(row, SESSION_IP)->addWidget(a);
        Wt::WText* t;
        UserPtr u = app->user();
        UserPtr user = u ? tApp->session().load<User>(u.id()) : UserPtr();
        elementAt(row, SESSION_USER)->addWidget(user_anchor(u));
        t = new Wt::WText(app->created().toString());
        elementAt(row, SESSION_CREATED)->addWidget(t);
        t = new Wt::WText(td2str(app->server_usage()));
        elementAt(row, SESSION_SERVER_USAGE)->addWidget(t);
        const std::string* cookie = app->environment().getCookieValue("userid");
        if (cookie) {
            t = new Wt::WText(*cookie);
            elementAt(row, SESSION_COOKIE)->addWidget(t);
        }
        const std::string& agent = app->environment().userAgent();
        t = new Wt::WText(agent);
        elementAt(row, SESSION_AGENT)->addWidget(t);
        row += 1;
    }
    setStyleClass("thechess-table-border");
}

}

