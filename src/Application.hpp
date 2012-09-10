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
#include <Wt/Wc/Gather.hpp>

#include "model/all.hpp"
#include "Server.hpp"
#include "Session.hpp"
#include "Path.hpp"

/** Macro for Application::instance(), same as wApp */
#define tApp thechess::Application::instance()

namespace thechess {

/** Descendant of Wt::WApplication.
\ingroup server
*/
class Application : public Wt::WApplication {
public:
    /** Constructor fot Wt::Application mode */
    Application(const Wt::WEnvironment& env, Server& server);

    /** Constructor fot Wt::WidgetSet mode.
    URL parameters:
     - type = game
     - entry = id of entry (e.g, game)
     - div = DOM id to be bound
    */
    Application(bool, const Wt::WEnvironment& env, Server& server);

    /** Destructor */
    virtual ~Application();

    /** Get session (database-related objects) */
    Session& session() {
        return session_;
    }

    /** Get path (url-related) */
    Path& path() {
        return path_;
    }

    /** Get application server */
    Server& server() {
        return server_;
    }

    /** Return user */
    const UserPtr& user() const {
        return user_;
    }

    /** Return user.
    This version for .reread().
    */
    UserPtr& user() {
        return user_;
    }

    /** Show Update Password dialog */
    void update_password();

    /** (Re)create or hide my games */
    void update_my_games();

    /** User set locale */
    void set_locale_by_user(const std::string& locale);

    /** Get active Application.
    Same as WApplication::instance().
    There is macro for this method: tApp.
    */
    static Application* instance();

protected:
    /** Notifies an event to the application.
    This function catches all exeptions.
    In case of dbo::StaleObjectException, all dbo objects are reread.
    Eceptions of other types cause application to quit().
    */
    virtual void notify(const Wt::WEvent& e);

private:
    Server& server_;
    Session session_;
    MainWidget* main_widget_;
    Path path_;
    UserPtr prev_user_;
    Wt::Wc::Gather* gather_;
    Kick* kick_;
    bool online_;
    Wt::WDateTime last_user_event_;
    Wt::WDateTime next_check_;
    UserPtr user_;

    void set_auth_widget();
    void login_handler();
    void gather_init();
    void gather_explorer(Wt::Wc::Gather::DataType type,
                         const std::string& value);
    void user_action();
    void online_check();
};

}

#endif

