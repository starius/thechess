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
#include <Wt/Auth/AuthWidget>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/SWFStore.hpp>
#include <Wt/Wc/Gather.hpp>

#include "Application.hpp"
#include "config.hpp"
#include "Session.hpp"
#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"

namespace thechess {

Application::Application(const Wt::WEnvironment& env, Server& server) :
    Wt::WApplication(env), server_(server), session_(server.pool()),
    gather_(0) {
    main_widget_ = new MainWidget(root());
    main_widget_->show_menu(&path_);
    path_.connect_main_widget(main_widget_);
    set_auth_widget();
    enableUpdates(true);
    useStyleSheet("css/1.css");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/thechess");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/wtclasses/wtclasses");
    setCssTheme("polished");
    session().login().changed().connect(this, &Application::login_handler);
    if (config::GATHERING) {
        server_.planning().schedule(10 * SECOND, Wt::Wc::bound_post(
                                        boost::bind(&Application::gather_init,
                                                this)));
    }
    login_handler();
    path_.open(internalPath());
}

Application::~Application() {
    try {
        dbo::Transaction t(session());
        user().reread();
        if (user()) {
            user().modify()->logout();
        }
        t.commit();
    } catch (std::exception& e) {
        log("warning") << e.what();
    }
}

UserPtr Application::user() {
    return session().user();
}

void Application::login_handler() {
    GamesVector games_vector;
    dbo::Transaction t(session());
    if (prev_user_ != user()) {
        prev_user_.reread();
        if (prev_user_) {
            prev_user_.modify()->logout();
        }
        user().reread();
        if (user()) {
            user().modify()->login();
            {
                Games games = user()->games().where("state in (?,?,?)")
                              .bind(Game::CONFIRMED)
                              .bind(Game::ACTIVE)
                              .bind(Game::PAUSE);
                games_vector.assign(games.begin(), games.end());
            }
            if (gather_) {
                gather_->explore_all();
            }
        }
        prev_user_ = user();
        path_.open(internalPath());
    }
    t.commit();
    BOOST_FOREACH (GamePtr game, games_vector) {
        server_.planning().add(new Object(GAME, game.id()), now());
    }
    main_widget_->main_menu()->show_user_items(session_.login().loggedIn());
    main_widget_->update_my_games();
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
    return downcast<Application*>(Wt::WApplication::instance());
}

void Application::set_auth_widget() {
    using namespace Wt::Auth;
    AuthWidget* w = new AuthWidget(server().auth_service(),
                                   session().user_database(),
                                   session().login());
    w->setRegistrationEnabled(true);
    w->addPasswordAuth(&server().password_service());
    try {
        w->processEnvironment();
    } catch (std::exception& e) {
        log("warning") << "AuthWidget.processEnvironment(): " << e.what();
    }
    main_widget_->set_auth_widget(w);
}

void Application::gather_init() {
    main_widget_->set_swfstore(new Wt::Wc::SWFStore());
    gather_ = new Wt::Wc::Gather(boost::bind(&Application::gather_explorer,
                                 this, _1, _2), this);
    gather_->set_swfstore(main_widget_->swf_store());
    triggerUpdate();
}

void Application::gather_explorer(Wt::Wc::Gather::DataType type,
                                  const std::string& value) {
}

}

