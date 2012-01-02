/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WAPPLICATION_HPP_
#define THECHESS_WAPPLICATION_HPP_

#include <vector>
#include <map>
#include <set>

#include <Wt/WGlobal>
#include <Wt/WApplication>

#include "model/all.hpp"
#include "Server.hpp"
#include "Session.hpp"
#include "Path.hpp"
#include "widgets/MainWidget.hpp"

/** Macro for Application::instance(), same as wApp */
#define tApp thechess::Application::instance()

namespace thechess {

/** Descendant of Wt::WApplication.
\ingroup server
*/
class Application : public Wt::WApplication {
public:
    Application(const Wt::WEnvironment& env, Server& server);
    virtual ~Application();

    Session& session() {
        return session_;
    }

    Path& path() {
        return path_;
    }

    Server& server() {
        return server_;
    }

    const UserPtr& user() const {
        return user_;
    }
    void set_user(const UserPtr& user);
    void logout();

    /** Get active Application.
    Same as WApplication::instance().
    There is macro for this method: tApp.
    */
    static Application* instance();

protected:
    virtual void notify(const Wt::WEvent& e);

private:
    Server& server_;
    Session session_;
    MainWidget* main_widget_;
    Path path_;
    UserPtr user_;

    void set_auth_widget();
};

}

#endif

