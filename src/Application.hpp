/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WAPPLICATION_HPP_
#define THECHESS_WAPPLICATION_HPP_

#include <vector>
#include <map>
#include <set>

namespace Wt {
class WEnvironment;
class WApplication;
class WContainerWidget;
class WImage;
class WBorderLayout;
class WWidget;
}
#include <Wt/WApplication>
#include <Wt/WSignal>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "model/all.hpp"
#include "Notifier.hpp"
#include "Server.hpp"
#include "Session.hpp"
#include "widgets/MainMenu.hpp"

/** Macro for Application::instance(), same as wApp */
#define tApp thechess::Application::instance()

namespace thechess {

class Session;

/** Base class for a widget, notifiable by thechess notifications.
\sa Application::thechess_notify
\sa Notifier
\ingroup server
*/
class Notifiable {
public:
    Notifiable(const Object& object);
    Notifiable(ObjectType ot, int id);

    virtual ~Notifiable();
    virtual void notify() = 0; // under Transaction

private:
    const Object object_;

    void add_to_application_();
};

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
    Server& server() {
        return server_;
    }

    const UserPtr& user() const {
        return user_;
    }
    void after_user_change_();
    void set_user(const UserPtr& user);
    void logout();

    void view(const UserPtr& user);
    void view(const GamePtr& game);
    void view(const CompetitionPtr& competition);

    template<typename M> void list_view() {
    }

    static void thechess_notify(Object object);

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
    UserPtr user_;
    Wt::WBorderLayout* layout_;
    typedef std::multimap<Object, Notifiable*> O2N;
    O2N notifiables_;
    bool active_;
    std::set<Notifiable*> waiting_notifiables_;
    const Object* notifying_object_;

    void add_notifiable_(Notifiable* notifiable,
                         const Object& object);
    void remove_notifiable_(Notifiable* notifiable,
                            const Object& object);

    void cookie_session_read_();
    void cookie_session_write_();

    void add_my_games_();
    void remove_my_games_();

    void onPathChange_();
    Wt::WContainerWidget* mainpanel_();
    void set_mainpanel_(Wt::WWidget* widget);

    template<typename W>
    void show_(std::string path) {
        setInternalPath(path);
        set_mainpanel_(new W());
    }

    template<typename W, typename I>
    void show_(I item, std::string path) {
        setInternalPath(path);
        set_mainpanel_(new W(item));
    }

    template<typename M>
    void object_view_(const char* path) {
        std::string id_str = internalPathNextPart(path);
        if (id_str.empty()) {
            list_view<M>();
        } else {
            int id = atoi(id_str.c_str());
            if (id) {
                dbo::Transaction t(tApp->session());
                dbo::ptr<M> o = tApp->session().find<M>()
                                .where("id = ?").bind(id);
                if (o) {
                    view(o);
                }
                t.commit();
            }
        }
    }

    friend class MainMenuImpl;
    friend class Notifiable;
};

template<> void Application::list_view<Game>();
template<> void Application::list_view<Competition>();

}

#endif

