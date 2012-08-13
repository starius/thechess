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
#include "Kick.hpp"
#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"

namespace thechess {

Application::Application(const Wt::WEnvironment& env, Server& server) :
    Wt::WApplication(env), server_(server), session_(server.pool()),
    gather_(0), kick_(0) {
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
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/wt");
    setCssTheme("polished");
    require("https://ajax.googleapis.com/ajax/libs/"
            "jquery/1.7.2/jquery.min.js", "jQuery");
    require("https://raw.github.com/starius/wt-classes/"
            "gh-pages/jquery.countdown.min.js");
    session().login().changed().connect(this, &Application::login_handler);
    login_handler();
    path_.open(internalPath());
}

Application::~Application() {
    delete kick_;
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

void Application::update_password() {
    dbo::Transaction t(session());
    if (user()) {
        main_widget_->update_password();
    }
    t.commit();
}

void Application::login_handler() {
    GamesVector games_vector;
    dbo::Transaction t(session());
    if (prev_user_ != user()) {
        prev_user_.reread();
        if (prev_user_) {
            delete kick_;
            kick_ = 0;
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
            if (config::GATHERING) {
                boost::function<void()> f = Wt::Wc::bound_post(boost::bind(
                                                &Application::gather_init,
                                                this));
                server_.planning().schedule(10 * SECOND, f);
            }
            kick_ = new Kick();
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
#if WT_MINOR==0x0
    // Wt 3.2.0
    AuthWidget* w = new AuthWidget(server().auth_service(),
                                   session().user_database(),
                                   session().login());
    w->addPasswordAuth(&server().password_service());
#else
    AuthModel* m = new AuthModel(server().auth_service(),
                                 session().user_database(),
                                 this);
    m->addPasswordAuth(&server().password_service());
    AuthWidget* w = new AuthWidget(session().login());
    w->setModel(m);
#endif
    w->setRegistrationEnabled(true);
    try {
        w->processEnvironment();
    } catch (std::exception& e) {
        log("warning") << "AuthWidget.processEnvironment(): " << e.what();
    }
    main_widget_->set_auth_widget(w);
}

void Application::gather_init() {
    if (!gather_) {
        main_widget_->set_swfstore(new Wt::Wc::SWFStore());
        gather_ = new Wt::Wc::Gather(boost::bind(&Application::gather_explorer,
                                     this, _1, _2), this);
        gather_->set_swfstore(main_widget_->swf_store());
        triggerUpdate();
    }
}

void Application::gather_explorer(Wt::Wc::Gather::DataType type,
                                  const std::string& value) {
    dbo::Transaction t(session());
    if (user()) {
        BDId bd_id(user(), type, value);
        BDPtr bd;
        try {
            bd = session().load<BD>(bd_id);
        } catch (...) {
            bd = session().add(new BD(bd_id));
        }
        bd.modify()->use();
    }
    t.commit();
}

}

