/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <string>
#include <stdexcept>
#include <boost/foreach.hpp>

#include <Wt/Dbo/backend/Sqlite3>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/Auth/Identity>

#include "Session.hpp"
#include "Options.hpp"
#include "Server.hpp"
#include "Planning.hpp"
#include "model/all.hpp"
#include "utils/utils.hpp"

namespace thechess {

Session::Session(dbo::FixedSqlConnectionPool& pool):
    user_database_(*this) {
    setConnectionPool(pool);
    mapClass<User>("thechess_user");
    mapClass<GP>("thechess_gp");
    mapClass<Game>("thechess_game");
    mapClass<CP>("thechess_cp");
    mapClass<Competition>("thechess_competition");
    mapClass<Comment>("thechess_comment");
    mapClass<BD>("thechess_bd");
    mapClass<IpBan>("thechess_ip_ban");
    mapClass<AuthInfo>("auth_info");
    mapClass<AuthInfo::AuthIdentityType>("auth_identity");
    mapClass<AuthInfo::AuthTokenType>("auth_token");
}

void Session::reconsider(Server& server) {
    Planning* planning = &server.planning();
    try {
        dbo::Transaction t(*this);
        createTables();
        std::cerr << "Created database" << std::endl;
        t.commit();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Using existing database" << std::endl;
    }
    {
        dbo::Transaction t(*this);
        UserPtr admin = add_user(server, "admin", "123");
        if (admin) {
            admin.modify()->set_rights(ADMIN);
        }
        add_user(server, "user", "123");
    }
    dbo::Transaction t(*this);
    execute("update thechess_user set sessions = ?").bind(0);
    t.commit();
    dbo::Transaction t2(*this);
    Games games = find<Game>().where("state < ?").bind(Game::MIN_ENDED);
    BOOST_FOREACH (const GamePtr& game, games) {
        planning->add(new Object(GAME, game.id()), now());
    }
    Competitions cs = find<Competition>()
                      .where("state < ?").bind(Competition::ENDED);
    BOOST_FOREACH (const CompetitionPtr& c, cs) {
        planning->add(new Object(COMPETITION, c.id()), now());
    }
    t2.commit();
}

dbo::SqlConnection* Session::new_connection(const Options& options) {
    dbo::SqlConnection* connection;
    if (options.database_type() == POSTGRES) {
        connection = new dbo::backend::Postgres(options.database_value());
    } else if (options.database_type() == SQLITE3) {
        std::string path = expand_path(options.database_value());
        connection = new dbo::backend::Sqlite3(path);
        connection->executeSql("PRAGMA synchronous = normal");
        connection->executeSql("PRAGMA journal_mode = wal");
    } else {
        throw std::logic_error("new_connection(): unknown database type");
    }
#ifndef NDEBUG
    connection->setProperty("show-queries", "true");
#endif
    return connection;
}

UserPtr Session::user() {
    dbo::Transaction t(*this);
    UserPtr user;
    if (login_.loggedIn()) {
        AuthInfoPtr auth_info = user_database_.find(login_.user());
        user = auth_info_to_user(auth_info);
    }
    t.commit();
    return user;
}

UserPtr Session::add_user(const Server& server, const Wt::WString& name,
                          const Wt::WString& password) {
    if (find<AuthInfo::AuthIdentityType>()
            .where("provider = 'loginname' and identity = ?")
            .bind(name).resultList().size()) {
        // already exists
        return UserPtr();
    }
    Wt::Auth::User user = user_database_.registerNew();
    user.addIdentity(Wt::Auth::Identity::LoginName, name);
    server.password_service().updatePassword(user, password);
    AuthInfoPtr auth_info = user_database_.find(user);
    return auth_info_to_user(auth_info);
}

UserPtr Session::auth_info_to_user(const AuthInfoPtr& auth_info) {
    UserPtr user = auth_info->user();
    if (!user) {
        Wt::WString name = auth_info->identity(Wt::Auth::Identity::LoginName);
        user = add(new User(name));
        auth_info.modify()->setUser(user);
    }
    return user;
}

}

