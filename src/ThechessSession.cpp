/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include <string>
#include <stdexcept>
#include <boost/foreach.hpp>

#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/backend/Postgres>

#include "ThechessSession.hpp"
#include "model/Object.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"
#include "model/Competition.hpp"
#include "model/CookieSession.hpp"
#include "TaskTracker.hpp"
#include "utils.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {
using namespace model;

ThechessSession::ThechessSession(dbo::FixedSqlConnectionPool& pool) {
    setConnectionPool(pool);
    mapClass<User>("thechess_user");
    mapClass<Game>("thechess_game");
    mapClass<CookieSession>("thechess_cookie_session");
    mapClass<Competition>("thechess_competition");
}

void ThechessSession::reconsider(TaskTracker& tracker) {
    try {
        dbo::Transaction t(*this);
        createTables();
        std::cerr << "Created database" << std::endl;

        User* admin = new User(true);
        admin->set_username("admin");
        admin->set_rights(User::admin);
        admin->set_password("123");
        add(admin);
        std::cerr<< "and admin user (password 123)" << std::endl;

        User* user = new User(true);
        user->set_username("user");
        user->set_password("123");
        add(user);
        std::cerr<< "and user user (password 123)" << std::endl;

        t.commit();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Using existing database" << std::endl;
    }

    dbo::Transaction t(*this);
    execute("update thechess_user set sessions = ?").bind(0);
    t.commit();

    dbo::Transaction t2(*this);
    Games games = find<Game>().where("state < ?").bind(Game::min_ended);
    BOOST_FOREACH(GamePtr game, games) {
        tracker.add_or_update_task(Object(GameObject, game.id()));
    }
    Competitions cs = find<Competition>().where("state < ?").bind(Competition::ENDED);
    BOOST_FOREACH(CompetitionPtr c, cs) {
        tracker.add_or_update_task(Object(CompetitionObject, c.id()));
    }
    t2.commit();
}

dbo::SqlConnection* ThechessSession::new_connection(const ThechessOptions& options) {
    dbo::SqlConnection* connection;
    if (options.database_type() == ThechessOptions::Postgres) {
        connection = new dbo::backend::Postgres(options.database_value());
    } else if (options.database_type() == ThechessOptions::Sqlite3) {
        std::string path = expand_path(options.database_value());
        connection = new dbo::backend::Sqlite3(path);
        connection->executeSql("PRAGMA synchronous = normal");
        connection->executeSql("PRAGMA journal_mode = wal");
    } else {
        throw std::logic_error("ThechessSession::new_connection(): unknown database type");
    }
#ifdef RUN_TESTS
    connection->setProperty("show-queries", "true");
#endif
    return connection;
}

}

