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

#include "ThechessNotifier.hpp"
#include "model/Object.hpp"
#include "model/User.hpp"
#include "model/Game.hpp"
#include "model/Competition.hpp"
#include "model/Object.hpp"
#include "ThechessServer.hpp"
#include "ThechessSession.hpp"
#include "widgets/MainMenu.hpp"

#define tApp ((ThechessApplication*)wApp)

namespace thechess {

using model::UserPtr;
using model::GamePtr;
using model::CompetitionPtr;

class ThechessSession;

class Notifiable {
public:
    Notifiable(const model::Object& object);
    Notifiable(model::ObjectType ot, int id);

    virtual ~Notifiable();
    virtual void notify()=0; // under Transaction

private:
    const model::Object object_;

    void add_to_application_();
};

class ThechessApplication : public Wt::WApplication {
public:
    ThechessApplication(const Wt::WEnvironment& env, ThechessServer& server);
    virtual ~ThechessApplication();

    ThechessSession& session() {
        return session_;
    }
    ThechessServer& server() {
        return server_;
    }

    UserPtr user() const {
        return user_;
    }
    void after_user_change_();
    void set_user(UserPtr user);
    void logout();

    void view(UserPtr user);
    void view(GamePtr game);
    void view(CompetitionPtr competition);

    template<typename M> void list_view() {
    }

    static void thechess_notify(model::Object object);

protected:
    virtual void notify(const Wt::WEvent& e);

private:
    ThechessServer& server_;
    ThechessSession session_;
    UserPtr user_;
    Wt::WBorderLayout* layout_;
    typedef std::multimap<model::Object, Notifiable*> O2N;
    O2N notifiables_;
    bool active_;
    std::set<Notifiable*> waiting_notifiables_;
    const model::Object* notifying_object_;

    void add_notifiable_(Notifiable* notifiable,
                         const model::Object& object);
    void remove_notifiable_(Notifiable* notifiable,
                            const model::Object& object);

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

    friend class widgets::MainMenuImpl;
    friend class Notifiable;
};

template<> void ThechessApplication::list_view<model::Game>();
template<> void ThechessApplication::list_view<model::Competition>();

}

#endif
