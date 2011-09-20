/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
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
#include <Wt/WApplication>
#include <Wt/WSignal>
#include <Wt/WText>
#include <Wt/WString>
#include <Wt/WContainerWidget>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;
#include <Wt/WWidget>
#include <Wt/WImage>
#include <Wt/WBorderLayout>
#include <Wt/WHBoxLayout>
#include <Wt/Dbo/Exception>
#include <Wt/WLogger>

#include "ThechessApplication.hpp"

#include "config.hpp"
#include "utils/time_intervals.hpp"
#include "widgets/RegisterWidget.hpp"
#include "widgets/LoginWidget.hpp"
#include "widgets/GameWidget.hpp"
#include "widgets/GameCreateWidget.hpp"
#include "widgets/CompetitionWidget.hpp"
#include "widgets/CompetitionCreateWidget.hpp"
#include "widgets/GameListWidget.hpp"
#include "widgets/CompetitionListWidget.hpp"
#include "widgets/UserWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "widgets/MyGamesList.hpp"
#include "ThechessSession.hpp"
#include "model/all.hpp"
#include "TaskTracker.hpp"

namespace thechess {

ThechessApplication::ThechessApplication(const Wt::WEnvironment& env, ThechessServer& server) :
    Wt::WApplication(env), server_(server), session_(server.pool()),
    active_(true), notifying_object_(0) {
    enableUpdates(true);
    useStyleSheet("css/1.css");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
                                "locales/thechess");
    internalPathChanged().connect(this,
                                  &ThechessApplication::onPathChange_);
    setCssTheme("polished");
    Wt::WContainerWidget* root1 = new Wt::WContainerWidget(root());
    layout_ = new Wt::WBorderLayout();
    root1->setLayout(layout_, Wt::AlignTop | Wt::AlignJustify);
    layout_->addWidget(new Wt::WImage("img/top.gif"),
                       Wt::WBorderLayout::North);
    layout_->addWidget(new Wt::WContainerWidget(),
                       Wt::WBorderLayout::Center);
    MainMenu* main_menu = new MainMenu();
    layout_->addWidget(main_menu, Wt::WBorderLayout::West);
    try {
        cookie_session_read_();
        onPathChange_();
    } catch (std::exception& e) {
        log("warning") << e.what();
    }
}

ThechessApplication::~ThechessApplication() {
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

void ThechessApplication::cookie_session_read_() {
    try {
        dbo::Transaction t(session());
        UserPtr u;
        std::string cookie_id = environment().getCookie("cookie_session");
        CookieSessionPtr cookie_session =
            session().load<CookieSession>(cookie_id);
        if (cookie_session->user()) {
            u = cookie_session->user();
        }
        cookie_session.modify()->use();
        t.commit();
        if (u) {
            set_user(cookie_session->user());
        }
    } catch (std::exception& e) {
        log("warning") << e.what();
    }
}

void ThechessApplication::cookie_session_write_() {
    dbo::Transaction t(session());
    std::string cookie_id;
    CookieSessionPtr cookie_session;
    try {
        cookie_id = environment().getCookie("cookie_session");
    } catch (std::runtime_error e) {
        cookie_id = sessionId();
    }
    try {
        cookie_session = session().load<CookieSession>(cookie_id);
    } catch (dbo::ObjectNotFoundException e) {
        cookie_session =
            session().add(new CookieSession(cookie_id));
    }
    setCookie("cookie_session", cookie_id, config::COOKIE_SESSION_AGE,
              "", makeAbsoluteUrl(""));
    cookie_session.modify()->set_user(user());
    cookie_session.modify()->use();
    t.commit();
}

void ThechessApplication::add_my_games_() {
    layout_->addWidget(new MyGamesList(user()), Wt::WBorderLayout::East);
}

void ThechessApplication::remove_my_games_() {
    Wt::WWidget* my_games_list = layout_->widgetAt(Wt::WBorderLayout::East);
    layout_->removeWidget(my_games_list);
    delete my_games_list;
}

void ThechessApplication::after_user_change_() {
    std::string path = internalPathNextPart("/");
    if (user() && (path == "login" || path == "register")) {
        view(user());
    } else if (path == "logout") {
        setInternalPath("/", true);
    } else {
        onPathChange_();
    }
}

void ThechessApplication::set_user(const UserPtr& user) {
    dbo::Transaction t(session());
    user_.reread();
    if (user_) {
        user_.modify()->logout();
        remove_my_games_();
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
    cookie_session_write_();
    after_user_change_();
    t.commit();
    add_my_games_();
}

void ThechessApplication::logout() {
    dbo::Transaction t(session());
    user_.reread();
    if (user_) {
        user_.modify()->logout();
        user_.reset();
        remove_my_games_();
    }
    cookie_session_write_();
    after_user_change_();
    t.commit();
}

void ThechessApplication::notify(const Wt::WEvent& e) {
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

Wt::WContainerWidget* ThechessApplication::mainpanel_() {
    return (Wt::WContainerWidget*)
           (layout_->widgetAt(Wt::WBorderLayout::Center));
}

void ThechessApplication::set_mainpanel_(Wt::WWidget* widget) {
    Wt::WContainerWidget* mp = mainpanel_();
    mp->clear();
    mp->addWidget(widget);
}

void ThechessApplication::onPathChange_() {
    std::cout << internalPath() << std::endl;
    std::string section = internalPathNextPart("/");
    if (section == "register") {
        set_mainpanel_(new RegisterWidget());
    } else if (section == "logout") {
        logout();
    } else if (section == "login") {
        set_mainpanel_(new LoginWidget());
    } else if (section == "user") {
        object_view_<User>("/user/");
    } else if (section == "game") {
        if (internalPathNextPart("/game/") == "challenge") {
            set_mainpanel_(new GameCreateWidget());
        } else {
            object_view_<Game>("/game/");
        }
    } else if (section == "competition") {
        if (internalPathNextPart("/competition/") == "new") {
            set_mainpanel_(new CompetitionCreateWidget());
        } else {
            object_view_<Competition>("/competition/");
        }
    } else {
        set_mainpanel_(new Wt::WContainerWidget()); // FIXME index page
    }
}

void ThechessApplication::view(const UserPtr& user) {
    show_<UserWidget>(user, str(boost::format("/user/%i/") % user.id()));
}

void ThechessApplication::view(const GamePtr& game) {
    show_<GameWidget>(game, str(boost::format("/game/%i/") % game.id()));
}

void ThechessApplication::view(const CompetitionPtr& competition) {
    show_<CompetitionWidget>(competition, str(boost::format("/competition/%i/") % competition.id()));
}

template<> void ThechessApplication::list_view<Game>() {
    show_<GameListWidget>("/game/");
}

template<> void ThechessApplication::list_view<Competition>() {
    show_<CompetitionListWidget>("/competition/");
}

void ThechessApplication::thechess_notify(Object object) {
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

void ThechessApplication::add_notifiable_(Notifiable* notifiable,
        const Object& object) {
    if (notifiables_.find(object) == notifiables_.end()) {
        // first Notifiable for the object is being created
        server_.notifier().start_listenning(object);
    }
    notifiables_.insert(O2N::value_type(object, notifiable));
}

void ThechessApplication::remove_notifiable_(Notifiable* notifiable,
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

Notifiable::Notifiable(const Object& object):
    object_(object) {
    if (tApp->active_) {
        // do not call after ~ThechessApplication
        tApp->add_notifiable_(this, object_);
    }
}

Notifiable::Notifiable(ObjectType ot, int id):
    object_(ot, id) {
    if (tApp->active_) {
        // do not call after ~ThechessApplication
        tApp->add_notifiable_(this, object_);
    }
}

Notifiable::~Notifiable() {
    if (tApp->active_) {
        // do not call after ~ThechessApplication
        tApp->remove_notifiable_(this, object_);
    }
}

}

