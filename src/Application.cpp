/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <utility>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>

#include <Wt/WEnvironment>
#include <Wt/Dbo/Exception>
#include <Wt/WLogger>

#include "Application.hpp"
#include "config.hpp"
#include "Session.hpp"
#include "model/all.hpp"
#include "TaskTracker.hpp"

namespace thechess {

Application::Application(const Wt::WEnvironment& env, Server& server) :
    Wt::WApplication(env), server_(server), session_(server.pool()),
    path_(),
    active_(true), notifying_object_(0) {
    main_widget_ = new MainWidget(root());
    main_widget_->show_menu(&path_);
    path_.connect_main_widget(main_widget_);
    enableUpdates(true);
    useStyleSheet("css/1.css");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/thechess");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/wtclasses/wtclasses");
    setCssTheme("polished");
}

Application::~Application() {
    std::set<Object> objects;
    for (O2N::iterator it = notifiables_.begin(); it != notifiables_.end(); ++it) {
        objects.insert(it->first);
    }
    BOOST_FOREACH (const Object& object, objects) {
        server_.notifier().stop_listenning(object);
    }
    notifiables_.clear();
    active_ = false;
    try {
        dbo::Transaction t(session());
        user_.reread();
        if (user_) {
            user_.modify()->logout();
        }
        t.commit();
    } catch (std::exception& e) {
        log("warning") << e.what();
    }
}

void Application::set_user(const UserPtr& user) {
    dbo::Transaction t(session());
    user_.reread();
    if (user_) {
        user_.modify()->logout();
    }
    user_ = user;
    user_.reread();
    user_.modify()->login();
    GamesVector games_vector;
    {
        Games games = user_->games().where("state in (?,?,?)")
                      .bind(Game::CONFIRMED)
                      .bind(Game::ACTIVE)
                      .bind(Game::PAUSE);
        games_vector.assign(games.begin(), games.end());
    }
    BOOST_FOREACH (GamePtr game, games_vector) {
        server_.tracker().add_or_update_task(Object(GAME, game.id()));
    }
    t.commit();
}

void Application::logout() {
    dbo::Transaction t(session());
    user_.reread();
    if (user_) {
        user_.modify()->logout();
        user_.reset();
    }
    t.commit();
}

void Application::notify(const Wt::WEvent& e) {
    try {
        Wt::WApplication::notify(e);
    } catch (dbo::StaleObjectException e) {
        log("notice") << e.what();
        try {
            dbo::Transaction t(session());
            session().rereadAll();
            t.commit();
        } catch (...) {
        }
    } catch (std::exception& e) {
        log("fatal") << e.what();
        quit();
    }
}

void Application::thechess_notify(Object object) {
    try {
        dbo::Transaction t(tApp->session());
        object.reread(tApp->session());
        std::pair<O2N::iterator, O2N::iterator> range =
            tApp->notifiables_.equal_range(object);
        std::set<Notifiable*>& waiting_notifiables = tApp->waiting_notifiables_;
        tApp->notifying_object_ = &object;
        waiting_notifiables.clear();
        for (O2N::iterator it = range.first; it != range.second; ++it) {
            Notifiable* notifiable = it->second;
            waiting_notifiables.insert(notifiable);
        }
        while (!waiting_notifiables.empty()) {
            std::set<Notifiable*>::iterator it = waiting_notifiables.begin();
            Notifiable* notifiable = *it;
            waiting_notifiables.erase(it);
            notifiable->notify();
        }
        tApp->notifying_object_ = 0;
        tApp->triggerUpdate();
        t.commit();
    } catch (std::exception& e) {
        tApp->log("warning") << e.what();
    }
}

void Application::add_notifiable_(Notifiable* notifiable,
                                  const Object& object) {
    if (notifiables_.find(object) == notifiables_.end()) {
        // first Notifiable for the object is being created
        server_.notifier().start_listenning(object);
    }
    notifiables_.insert(O2N::value_type(object, notifiable));
}

void Application::remove_notifiable_(Notifiable* notifiable,
                                     const Object& object) {
    O2N::iterator to_delete;
    std::pair<O2N::iterator, O2N::iterator> range =
        notifiables_.equal_range(object);
    for (to_delete = range.first; to_delete != range.second; ++to_delete) {
        if (to_delete->second == notifiable) {
            break;
        }
    }
    BOOST_ASSERT(to_delete->second == notifiable);
    if (notifying_object_) {
        waiting_notifiables_.erase(notifiable);
    }
    notifiables_.erase(to_delete);
    if (notifiables_.find(object) == notifiables_.end()) {
        // last Notifiable for the object is being deleted
        server_.notifier().stop_listenning(object);
    }
}

Application* Application::instance() {
    return static_cast<Application*>(Wt::WApplication::instance());
}

Notifiable::Notifiable(const Object& object):
    object_(object) {
    if (tApp->active_) {
        // do not call after ~Application
        tApp->add_notifiable_(this, object_);
    }
}

Notifiable::Notifiable(ObjectType ot, int id):
    object_(ot, id) {
    if (tApp->active_) {
        // do not call after ~Application
        tApp->add_notifiable_(this, object_);
    }
}

Notifiable::~Notifiable() {
    if (tApp->active_) {
        // do not call after ~Application
        tApp->remove_notifiable_(this, object_);
    }
}

}

