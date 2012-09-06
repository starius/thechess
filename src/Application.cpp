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
#include <Wt/WText>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/SWFStore.hpp>
#include <Wt/Wc/Gather.hpp>

#include "Application.hpp"
#include "config.hpp"
#include "Kick.hpp"
#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"

namespace thechess {

Application::Application(const Wt::WEnvironment& env, Server& server) :
    Wt::WApplication(env), server_(server), session_(server.pool()),
    gather_(0), kick_(0),
    online_(true), last_user_event_(now()), next_check_(now()) {
    main_widget_ = new MainWidget(root());
    main_widget_->show_menu(&path_);
    path_.connect_main_widget(main_widget_);
    path_.error404().connect(main_widget_, &MainWidget::main_page);
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
    internalPathChanged().connect(this, &Application::user_action);
}

Application::~Application() {
    delete kick_;
    try {
        dbo::Transaction t(session());
        user().reread();
        if (user() && online_) {
            user().modify()->logout();
        }
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
}

static void check_session_number() {
    dbo::Transaction t(tApp->session());
    if (tApp && tApp->user() && tApp->user()->sessions() > 10) {
        tApp->root()->clear();
        new Wt::WText(Wt::WString::tr("tc.user.Many_sessions"), tApp->root());
        tApp->quit();
    }
}

void Application::login_handler() {
    GamesVector games_vector;
    dbo::Transaction t(session());
    if (prev_user_ != user()) {
        prev_user_.reread();
        if (prev_user_) {
            delete kick_;
            kick_ = 0;
            if (online_) {
                prev_user_.modify()->logout();
            }
            prev_user_.flush();
        }
        user().reread();
        if (user()) {
            user().modify()->login();
            online_ = true;
            Wt::Wc::bound_post(check_session_number)();
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
            user().flush();
        }
        prev_user_ = user();
        path_.open(internalPath());
    }
    t.commit();
    BOOST_FOREACH (GamePtr game, games_vector) {
        server_.planning().add(new Object(GAME, game.id()), now());
    }
    main_widget_->main_menu()->show_user_items(session_.login().loggedIn());
    main_widget_->set_top_block_shown(session_.login().loggedIn());
    main_widget_->update_my_games();
}

void Application::notify(const Wt::WEvent& e) {
    try {
        Wt::EventType e_type = e.eventType();
        Wt::WApplication::notify(e);
        if (e_type == Wt::UserEvent) {
            user_action();
        }
    } catch (dbo::StaleObjectException e) {
        log("notice") << e.what();
        try {
            dbo::Transaction t(session());
            session().rereadAll();
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
    main_widget_->set_auth_widget();
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
    if (type == Wt::Wc::Gather::IP && value == "127.0.0.1") {
        return;
    }
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
}

void Application::user_action() {
    last_user_event_ = now();
    if (!online_) {
        dbo::Transaction t(session());
        online_ = true;
        user().reread();
        if (user()) {
            user().modify()->login();
        }
    }
    if (now() > next_check_) {
        Td timeout = Options::instance()->away_timeout();
        Td td = timeout + SECOND;
        next_check_ = now() + td;
        Wt::Wc::schedule_action(td, Wt::Wc::bound_post(boost::bind(
                                    &Application::online_check, this)));
    }
}

void Application::online_check() {
    Td timeout = Options::instance()->away_timeout();
    if (now() - last_user_event_ > timeout) {
        if (online_) {
            dbo::Transaction t(session());
            user().reread();
            if (user()) {
                online_ = false;
                user().modify()->logout();
            }
        }
    } else {
        next_check_ = last_user_event_ + timeout + SECOND;
        Td td = next_check_ - now();
        Wt::Wc::schedule_action(td, Wt::Wc::bound_post(boost::bind(
                                    &Application::online_check, this)));
    }
}

}

