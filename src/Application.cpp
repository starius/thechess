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
#include <Wt/Wc/LocalStore.hpp>
#include <Wt/Wc/Gather.hpp>

#include "Application.hpp"
#include "Server.hpp"
#include "config.hpp"
#include "Kick.hpp"
#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "widget-set.hpp"
#include "log.hpp"

namespace thechess {

typedef std::map<std::string, int> Ip2Int;
static Ip2Int sessions_per_ip_;
typedef std::map<Application*, App> AppMap;
static AppMap app_map_;
static boost::mutex sessions_per_ip_mutex_;

Application::Application(const Wt::WEnvironment& env, Server& server) :
    Wt::WApplication(env), server_(server), session_(server.pool()),
    gather_(0), kick_(0),
    server_usage_(0), timezone_signal_(root(), "timezone") {
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
    require("/js/jquery.min.js");
    require("/js/jquery.countdown.min.js");
    // FIXME http://redmine.emweb.be/issues/1491
    doJavaScript("window.alert = $.noop;");
    Wt::Wc::fix_plain_anchors(/* external_blank */ true);
    session().login().changed().connect(this, &Application::login_handler);
    login_handler();
    path_.open(internalPath());
    internalPathChanged().connect(this, &Application::user_action);
    explore_timezone();
}

Application::Application(bool, const Wt::WEnvironment& env, Server& server):
    Wt::WApplication(env), server_(server), session_(server.pool()),
    gather_(0), kick_(0),
    server_usage_(0), timezone_signal_(root(), "timezone") {
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
    // FIXME http://redmine.emweb.be/issues/1491
    doJavaScript("window.alert = $.noop;");
    init_widget_mode();
    explore_timezone();
}

Application::~Application() {
    decrease_sessions_counter();
    delete kick_;
}

std::vector<App> Application::all_sessions() {
    boost::mutex::scoped_lock lock(sessions_per_ip_mutex_);
    std::vector<App> result;
    BOOST_FOREACH (const AppMap::value_type& id_and_app, app_map_) {
        result.push_back(id_and_app.second);
    }
    return result;
}

int Application::sessions_number() {
    boost::mutex::scoped_lock lock(sessions_per_ip_mutex_);
    return app_map_.size();
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
    {
        boost::mutex::scoped_lock lock(sessions_per_ip_mutex_);
        app_map_[this].user = user_;
    }
    bool show_user_items = true;
    if (prev_user_ != user()) {
        prev_user_.reread();
        user().reread();
        if (user()) {
            if (user()->has_comment_base() && !user()->online()) {
                int new_messages = user()->comment_base()->children().find()
                                   .where("created > ?")
                                   .bind(user()->last_online())
                                   .resultList().size();
                if (new_messages) {
                    main_widget_->main_menu()->show_user_items(true);
                    t_emit(new NewMessage(user_.id()));
                    show_user_items = false;
                }
            }
            user_action();
            check_my_games();
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
    if (show_user_items) {
        main_widget_->main_menu()->show_user_items(session_.login().loggedIn());
    }
    main_widget_->set_top_block_shown(session_.login().loggedIn());
    main_widget_->update_my_games();
}

void Application::notify(const Wt::WEvent& e) {
    try {
        Wt::EventType e_type = e.eventType();
        Wt::WDateTime start = now();
        Wt::WApplication::notify(e);
        Wt::WDateTime stop = now();
        if (server_usage_) {
            *server_usage_ += stop - start;
        }
        if (e_type == Wt::UserEvent) {
            // FIXME this does not work
            user_action();
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

void Application::explore_timezone() {
    timezone_signal_.connect(this, &Application::set_timezone_diff);
    doJavaScript(timezone_signal_.createCall(
                     "(new Date()).getTimezoneOffset()"));
}

void Application::set_timezone_diff(int shift) {
    shift = Wt::Wc::constrained_value(-24*60, shift, 24*60);
    timezone_diff_ = -shift * MINUTE;
    path().open(internalPath());
}

bool Application::check_ip() {
    ip_ = environment().clientAddress();
    bool byu;
    {
        boost::mutex::scoped_lock lock(sessions_per_ip_mutex_);
        int& count = sessions_per_ip_[ip_];
        count += 1;
        App& app = app_map_[this];
        app.created = now();
        app.server_usage = TD_NULL;
        server_usage_ = &app.server_usage;
        app.ip = ip_;
        const std::string* cookie = environment().getCookieValue("userid");
        if (cookie) {
            app.cookie = *cookie;
        }
        app.agent = environment().userAgent();
        const Options* o = Options::instance();
        bool white = o->ip_in_whitelist(ip_);
        int max_s = white ? o->whitelist_max_sessions() : o->max_sessions();
        byu = count > max_s;
    }
    if (byu) {
        redirect("/html/too_many_sessions.html");
        quit();
        try {
            admin_log("Too many sessions from IP " + ip_a(ip_), true);
        } catch (...)
        { }
        return false;
    } else if (IpBan::am_i_banned() >= ABSOLUTE_BAN) {
        redirect("/html/banned.html");
        quit();
        try {
            admin_log("Attempt to enter from banned IP " + ip_a(ip_), true);
        } catch (...)
        { }
        return false;
    } else {
        return true;
    }
}

void Application::decrease_sessions_counter() {
    boost::mutex::scoped_lock lock(sessions_per_ip_mutex_);
    Ip2Int::iterator it = sessions_per_ip_.find(ip_);
    if (it != sessions_per_ip_.end()) {
        it->second -= 1;
        if (it->second <= 0) {
            sessions_per_ip_.erase(it);
        }
    }
    app_map_.erase(this);
    server_usage_ = 0;
}

void Application::set_auth_widget() {
    main_widget_->set_auth_widget();
}

void Application::gather_init() {
    if (!gather_) {
        gather_ = new Wt::Wc::Gather(boost::bind(&Application::gather_explorer,
                                     this, _1, _2), this);
        gather_->set_swfstore(new Wt::Wc::SWFStore(root()));
        gather_->set_localstore(new Wt::Wc::LocalStore(root()));
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
    dbo::Transaction t(session());
    if (user()) {
        user().reread();
        Td away = now() - user()->last_online();
        float away_ratio = away / Options::instance()->away_timeout();
        if (away_ratio > 0.5) {
            user().modify()->update_last_online();
        }
        if (away_ratio > 2) {
            check_my_games();
        }
    }
}

void Application::check_my_games() {
    dbo::Transaction t(session());
    if (user()) {
        Games games = user()->games().where("state = ?").bind(Game::CONFIRMED);
        BOOST_FOREACH (GamePtr game, games) {
            t_task(GAME, game.id());
        }
    }
}

}

