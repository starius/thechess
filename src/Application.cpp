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
    Wt::WApplication(env), server_(server), session_(server.pool()) {
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

Application* Application::instance() {
    return static_cast<Application*>(Wt::WApplication::instance());
}

}

