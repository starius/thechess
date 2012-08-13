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
    /** Constructor */
    Application(const Wt::WEnvironment& env, Server& server);

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
    UserPtr user();

    /** Show Update Password dialog */
    void update_password();

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

    void set_auth_widget();
    void login_handler();
    void gather_init();
    void gather_explorer(Wt::Wc::Gather::DataType type,
                         const std::string& value);
};

}

#endif

