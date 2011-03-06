
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <cstdlib>

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
#include <stdexcept>

#include "ThechessApplication.hpp"

#include "config.hpp"
#include "time_intervals.hpp"
#include "widgets/RegisterWidget.hpp"
#include "widgets/LoginWidget.hpp"
#include "widgets/GameWidget.hpp"
#include "widgets/GameCreateWidget.hpp"
#include "widgets/GameListWidget.hpp"
#include "widgets/UserWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "ThechessSession.hpp"
#include "model/User.hpp"
#include "model/CookieSession.hpp"
#include "model/Game.hpp"
#include "TaskTracker.hpp"

namespace thechess {

using model::UserPtr;
using model::User;
using model::GamePtr;
using model::Game;

ThechessSession ThechessApplication::session_;

ThechessApplication::ThechessApplication(const Wt::WEnvironment& env) :
Wt::WApplication(env)
{
    enableUpdates(true);
    useStyleSheet("css/1.css");
    messageResourceBundle().use(Wt::WApplication::appRoot() +
            "locales/thechess");
    internalPathChanged().connect(this,
            &ThechessApplication::onPathChange_);
    setCssTheme("polished");

    Wt::WBorderLayout* layout = new Wt::WBorderLayout();
    root()->setLayout(layout, Wt::AlignTop | Wt::AlignJustify);
    layout->addWidget(new Wt::WImage("/img/top.gif"),
            Wt::WBorderLayout::North);
    layout->addWidget(new Wt::WContainerWidget(),
            Wt::WBorderLayout::Center);

    widgets::MainMenu* main_menu = new widgets::MainMenu();
    layout->addWidget(main_menu, Wt::WBorderLayout::West);

    #ifdef RUN_TESTS
    #endif
    
    cookie_session_read_();

    onPathChange_();
}

ThechessApplication::~ThechessApplication()
{
    dbo::Transaction t(tApp->session());
    if (user_)
    {
        user_.modify()->logout();
    }
    t.commit();
}

void ThechessApplication::consider_db()
{
    try
    {
        dbo::Transaction t(session());
        session().createTables();
        std::cerr << "Created database" << std::endl;

        User* admin = new User();
        admin->set_username("admin");
        admin->set_rights(User::admin);
        admin->set_password("123");
        session().add(admin);
        std::cerr<< "and admin user (password 123)" << std::endl;

        User* user = new User();
        user->set_username("user");
        user->set_password("123");
        session().add(user);
        std::cerr<< "and user user (password 123)" << std::endl;

        t.commit();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "Using existing database" << std::endl;
    }

    dbo::Transaction t(session());
    session().execute("update thechess_user set sessions = ?").bind(0);

    model::Games games =
        session().find<Game>().where("state < ?").bind(Game::min_ended);
    BOOST_FOREACH(GamePtr game, games)
    {
        tracker::add_or_update_task(tracker::Game, game->id());
    }
    t.commit();
}

void ThechessApplication::cookie_session_read_()
{
    dbo::Transaction t(tApp->session());
    try
    {
        std::string cookie_id = environment().getCookie("cookie_session");
        model::CookieSessionPtr cookie_session =
            session().load<model::CookieSession>(cookie_id);
        if (cookie_session->user())
        {
            set_user(cookie_session->user());
        }
        cookie_session.modify()->use();
    }
    catch (...)
    {
    }
    t.commit();
}

void ThechessApplication::cookie_session_write_()
{
    dbo::Transaction t(tApp->session());
    std::string cookie_id;
    model::CookieSessionPtr cookie_session;
    try
    {
        cookie_id = environment().getCookie("cookie_session");
    }
    catch (std::runtime_error e)
    {
        cookie_id = sessionId();
    }
    try
    {
        cookie_session = session().load<model::CookieSession>(cookie_id);
    }
    catch (dbo::ObjectNotFoundException e)
    {
        cookie_session =
            session().add(new model::CookieSession(cookie_id));
    }
    setCookie("cookie_session", cookie_id, (365*day).total_seconds());
    cookie_session.modify()->set_user(user());
    cookie_session.modify()->use();
    t.commit();
}

void ThechessApplication::after_user_change_()
{
    std::string path = internalPathNextPart("/");
    if (user() && (path == "login" || path == "register"))
    {
        view(user());
    }
    else
    {
        onPathChange_();
    }
}

void ThechessApplication::set_user(UserPtr user)
{
    using model::Game;
    dbo::Transaction t(tApp->session());
    if (user_)
    {
        user_.modify()->logout();
    }
    user_ = user;
    user_.modify()->login();
    model::Games games = user_->games().where("state in (?,?,?)")
        .bind(Game::confirmed)
        .bind(Game::active)
        .bind(Game::pause);
    BOOST_FOREACH(model::GamePtr game, games)
    {
        game.modify()->check();
    }
    cookie_session_write_();
    after_user_change_();
    t.commit();
}

void ThechessApplication::logout()
{
    dbo::Transaction t(tApp->session());
    if (user_)
    {
        user_.modify()->logout();
        user_.reset();
    }
    cookie_session_write_();
    after_user_change_();
    t.commit();
}

Wt::WBorderLayout* ThechessApplication::layout_()
{
    return (Wt::WBorderLayout*)(root()->layout());
}

Wt::WContainerWidget* ThechessApplication::mainpanel_()
{
    return (Wt::WContainerWidget*)
        (layout_()->widgetAt(Wt::WBorderLayout::Center));
}

void ThechessApplication::set_mainpanel_(Wt::WWidget* widget)
{
    Wt::WContainerWidget* mp = mainpanel_();
    mp->clear();
    mp->addWidget(widget);
}

void ThechessApplication::onPathChange_()
{
    std::cout << internalPath() << std::endl;
    std::string section = internalPathNextPart("/");
    if (section == "register")
    {
        set_mainpanel_(new widgets::RegisterWidget());
    }
    if (section == "login")
    {
        set_mainpanel_(new widgets::LoginWidget());
    }
    if (section == "user")
    {
        object_view_<model::User>("/user/");
    }
    if (section == "game")
    {
        if (internalPathNextPart("/game/") == "challenge")
        {
            set_mainpanel_(new widgets::GameCreateWidget());
        }
        object_view_<model::Game>("/game/");
    }
}


void ThechessApplication::view(UserPtr user)
{
    show_<widgets::UserWidget>(user,
        str(boost::format("/user/%i/") % user.id()));
}

void ThechessApplication::view(GamePtr game)
{
    show_<widgets::GameWidget>(game, 
        str(boost::format("/game/%i/") % game.id()));
}

template<> void ThechessApplication::list_view<model::Game>()
{
    show_<widgets::GameListWidget>("/game/");
}

}
