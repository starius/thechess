/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <stdexcept>
#include <vector>
#include <map>
#include <utility>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/assert.hpp>
#include <boost/thread/mutex.hpp>

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
#include "widget-set.hpp"

namespace thechess {

typedef std::map<std::string, int> Ip2Int;
static Ip2Int sessions_per_ip_;
static boost::mutex sessions_per_ip_mutex_;

Application::Application(const Wt::WEnvironment& env, Server& server) :
    Wt::WApplication(env), server_(server), session_(server.pool()),
    gather_(0), kick_(0),
    online_(true), last_user_event_(now()), next_check_(now()) {
    if (!check_ip()) {
        return;
    }
    main_widget_ = new MainWidget(root());
    main_widget_->add_locale_setters(boost::bind(
                                         &Application::set_locale_by_user,
                                         this, _1));
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
    require("/js/jquery.countdown.min.js");
    session().login().changed().connect(this, &Application::login_handler);
    login_handler();
    path_.open(internalPath());
    //internalPathChanged().connect(this, &Application::user_action);
}

Application::Application(bool, const Wt::WEnvironment& env, Server& server):
    Wt::WApplication(env), server_(server), session_(server.pool()),
    gather_(0), kick_(0),
    online_(true), last_user_event_(now()), next_check_(now()) {
    if (!check_ip()) {
        return;
    }
    enableUpdates(true);
    useStyleSheet("css/1.css");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/thechess");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/wtclasses/wtclasses");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/wt");
    // FIXME Wt::Auth translations in separate file do not work
    setCssTheme("polished");
    init_widget_mode();
}

Application::~Application() {
    decrease_sessions_counter();
    delete kick_;
    try {
        dbo::Transaction t(session());
        user().reread();
        if (user() && online_) {
            user().modify()->try_again_logout();
        }
    } catch (std::exception& e) {
        log("warning") << e.what();
    }
}

void Application::update_password() {
    dbo::Transaction t(session());
    if (user()) {
        main_widget_->update_password();
    }
}

void Application::update_my_games() {
    main_widget_->update_my_games();
}

void Application::set_locale_by_user(const std::string& locale) {
    setLocale(locale);
    dbo::Transaction t(tApp->session());
    if (user()) {
        user().reread();
        user().modify()->set_locale(locale);
    }
}

void Application::login_handler() {
    dbo::Transaction t(session());
    user_ = session().user();
    if (prev_user_ != user()) {
        prev_user_.reread();
        if (prev_user_) {
            delete kick_;
            kick_ = 0;
            if (online_) {
                prev_user_.modify()->try_again_logout();
            }
            prev_user_.flush();
        }
        user().reread();
        if (user()) {
            user().modify()->try_again_login();
            online_ = true;
            if (gather_) {
                gather_->explore_all();
            }
            if (config::GATHERING) {
                boost::function<void()> f = Wt::Wc::bound_post(boost::bind(
                                                &Application::gather_init,
                                                this));
                Wt::Wc::schedule_action(10 * SECOND, f);
            }
            kick_ = new Kick();
            if (!user()->locale().empty()) {
                setLocale(user()->locale());
            }
            user().flush();
        }
        prev_user_ = user();
        path_.open(internalPath());
    }
    t.commit();
    main_widget_->main_menu()->show_user_items(session_.login().loggedIn());
    main_widget_->set_top_block_shown(session_.login().loggedIn());
    main_widget_->update_my_games();
}

void Application::notify(const Wt::WEvent& e) {
    try {
        Wt::EventType e_type = e.eventType();
        Wt::WApplication::notify(e);
        if (e_type == Wt::UserEvent) {
            // FIXME this does not work
            //user_action();
        }
        // } catch (dbo::StaleObjectException e) {
        //     log("notice") << e.what();
        //     try {
        //         dbo::Transaction t(session());
        //         // session().rereadAll();
        //         // FIXME rereadAll() causes segfault on SettingsWidget
        //         // if tApp->user() is stale
        //     } catch (...) {
        //     }
        //     quit();
    } catch (std::exception& e) {
        log("fatal") << e.what();
        redirect(internalPath());
        quit();
    }
}

Application* Application::instance() {
    return downcast<Application*>(Wt::WApplication::instance());
}

bool Application::check_ip() {
    boost::mutex::scoped_lock lock(sessions_per_ip_mutex_);
    int& count = sessions_per_ip_[environment().clientAddress()];
    count += 1;
    if (count > Options::instance()->max_sessions()) {
        redirect("/html/too_many_sessions.html");
        quit();
        return false;
    } else {
        return true;
    }
}

void Application::decrease_sessions_counter() {
    boost::mutex::scoped_lock lock(sessions_per_ip_mutex_);
    Ip2Int::iterator it = sessions_per_ip_.find(environment().clientAddress());
    if (it != sessions_per_ip_.end()) {
        it->second -= 1;
        if (it->second <= 0) {
            sessions_per_ip_.erase(it);
        }
    }
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
    // this never called
    last_user_event_ = now();
    if (!online_) {
        dbo::Transaction t(session());
        online_ = true;
        user().reread();
        if (user()) {
            user().modify()->try_again_login();
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
    // this never called
    Td timeout = Options::instance()->away_timeout();
    if (now() - last_user_event_ > timeout) {
        if (online_) {
            dbo::Transaction t(session());
            user().reread();
            if (user()) {
                online_ = false;
                user().modify()->try_again_logout();
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

