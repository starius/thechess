
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

ThechessSession* ThechessApplication::session_=0;

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

    Wt::WContainerWidget* root1 = new Wt::WContainerWidget(root());
    layout_ = new Wt::WBorderLayout();
    root1->setLayout(layout_, Wt::AlignTop | Wt::AlignJustify);
    layout_->addWidget(new Wt::WImage("img/top.gif"),
            Wt::WBorderLayout::North);
    layout_->addWidget(new Wt::WContainerWidget(),
            Wt::WBorderLayout::Center);

    widgets::MainMenu* main_menu = new widgets::MainMenu();
    layout_->addWidget(main_menu, Wt::WBorderLayout::West);

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

Wt::WContainerWidget* ThechessApplication::mainpanel_()
{
    return (Wt::WContainerWidget*)
        (layout_->widgetAt(Wt::WBorderLayout::Center));
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
