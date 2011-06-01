
#include <string>
#include <boost/foreach.hpp>

#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/backend/Postgres>

#include "ThechessSession.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"
#include "model/Competition.hpp"
#include "model/CookieSession.hpp"
#include "TaskTracker.hpp"
#include "utils.hpp"

namespace dbo = Wt::Dbo;

namespace thechess
{

using model::User;
using model::Game;
using model::Competition;
using model::GamePtr;
using model::CookieSession;

dbo::FixedSqlConnectionPool* ThechessSession::pool_ = 0;

ThechessSession::ThechessSession()
{
    setConnectionPool(*pool_);
    mapClass<User>("thechess_user");
    mapClass<Game>("thechess_game");
    mapClass<CookieSession>("thechess_cookie_session");
    mapClass<Competition>("thechess_competition");
}

void ThechessSession::reconsider()
{
    try
    {
        dbo::Transaction t(*this);
        createTables();
        std::cerr << "Created database" << std::endl;

        User* admin = User::create_new();
        admin->set_username("admin");
        admin->set_rights(User::admin);
        admin->set_password("123");
        add(admin);
        std::cerr<< "and admin user (password 123)" << std::endl;

        User* user = User::create_new();
        user->set_username("user");
        user->set_password("123");
        add(user);
        std::cerr<< "and user user (password 123)" << std::endl;

        t.commit();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << "Using existing database" << std::endl;
    }

    dbo::Transaction t(*this);
    execute("update thechess_user set sessions = ?").bind(0);

    model::Games games = find<Game>().where("state < ?").bind(Game::min_ended);
    BOOST_FOREACH(GamePtr game, games)
    {
        tracker::add_or_update_task(tracker::Game, game->id(), this);
    }
    t.commit();
}

void ThechessSession::set_options(const ThechessOptions* options)
{
    dbo::SqlConnection* connection;
    if (options->database_type() == ThechessOptions::Postgres)
    {
        connection = new dbo::backend::Postgres(options->database_value());
    }
    else if (options->database_type() == ThechessOptions::Sqlite3)
    {
        std::string path = expand_path(options->database_value());
        connection = new dbo::backend::Sqlite3(path);
    }
    pool_ = new dbo::FixedSqlConnectionPool(connection,
        options->connections_in_pool());
}

}

