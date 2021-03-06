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
#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "Session.hpp"
#include "Options.hpp"
#include "Server.hpp"
#include "model/all.hpp"
#include "utils/utils.hpp"
#include "log.hpp"

namespace thechess {

Session::Session(dbo::FixedSqlConnectionPool& pool):
    user_database_(*this) {
    setConnectionPool(pool);
    map_classes();
}

Session::Session(dbo::SqlConnection& connection):
    user_database_(*this) {
    setConnection(connection);
    map_classes();
}

void Session::reconsider(Server& server) {
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
    dbo::Transaction t2(*this);
    Games games = find<Game>().where("state < ?").bind(Game::MIN_ENDED);
    BOOST_FOREACH (const GamePtr& game, games) {
        t_task(GAME, game.id());
    }
    Competitions cs = find<Competition>()
                      .where("state < ?").bind(Competition::ENDED);
    BOOST_FOREACH (const CompetitionPtr& c, cs) {
        t_task(COMPETITION, c.id());
    }
    Users users = find<User>().where("vacation_until is not null");
    BOOST_FOREACH (const UserPtr& user, users) {
        t_task(USER, user.id());
    }
    IpBans ip_bans = find<IpBan>();
    BOOST_FOREACH (const IpBanPtr& ban, ip_bans) {
        t_task(IP_BAN, ban.id());
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
    connection->setProperty("enable_nestloop", "on");
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

void Session::recalculate_game_rating() {
    dbo::Transaction t(*this);
    execute("update thechess_user set games_stat_elo = ?, games_stat_all = 0, "
            "games_stat_wins = 0, games_stat_fails = 0")
    .bind(EloPlayer(true).elo());
    int games_size = find<Game>().resultList().size();
    t.commit();
    const int STEP = 1000000;
    for (int offset = 0; offset < games_size; offset += STEP) {
        dbo::Transaction t2(*this);
        Games games = find<Game>().orderBy("ended").limit(STEP).offset(offset);
        BOOST_FOREACH (GamePtr game, games) {
            if (game.id() % 1000 == 0) {
                std::cerr << "Recalc. game " << game.id() << std::endl;
            }
            game->stat_change();
        }
    }
}

void Session::recalculate_competition_rating() {
    dbo::Transaction t(*this);
    execute("update thechess_user set compteitions_stat_elo = ?, "
            "compteitions_stat_all = 0, "
            "compteitions_stat_wins = 0, compteitions_stat_fails = 0")
    .bind(EloPlayer(true).elo());
    t.commit();
    dbo::Transaction t2(*this);
    Competitions ccc = find<Competition>()
                       .orderBy("ended").resultList();
    BOOST_FOREACH (CompetitionPtr c, ccc) {
        c->stat_change();
    }
}

void Session::recalculate_gp_games_size() {
    dbo::Transaction t(*this);
    GPs gps = find<GP>().resultList();
    BOOST_FOREACH (GPPtr gp, gps) {
        gp.modify()->set_games_size(gp->games().size());
        gp.purge();
    }
}

void Session::recalculate_cp_competitions_size() {
    dbo::Transaction t(*this);
    CPs cps = find<CP>().resultList();
    BOOST_FOREACH (CPPtr cp, cps) {
        cp.modify()->set_competitions_size(cp->competitions().size());
        cp.purge();
    }
}

void Session::recalculate_comments() {
    dbo::Transaction t(*this);
    execute("update thechess_comment set show_index = 1, depth = 1 "
            "where type = ?").bind(Comment::FORUM_POST_TEXT);
    execute("update thechess_comment set show_index = 0, depth = 0 "
            "where type = ?").bind(Comment::FORUM_COMMENT);
    t.commit();
    dbo::Transaction t2(*this);
    Comments comments = find<Comment>()
                        .where("type = ?").bind(Comment::FORUM_COMMENT)
                        .orderBy("id");
    BOOST_FOREACH (CommentPtr comment, comments) {
        comment.modify()->set_depth();
        comment.modify()->set_index();
        comment.purge();
    }
    t2.commit();
}

void Session::map_classes() {
    mapClass<User>("thechess_user");
    mapClass<GP>("thechess_gp");
    mapClass<Game>("thechess_game");
    mapClass<CP>("thechess_cp");
    mapClass<Competition>("thechess_competition");
    mapClass<Team>("thechess_team");
    mapClass<TCM>("thechess_tcm");
    mapClass<Comment>("thechess_comment");
    mapClass<BD>("thechess_bd");
    mapClass<IpBan>("thechess_ip_ban");
    mapClass<AuthInfo>("auth_info");
    mapClass<AuthInfo::AuthIdentityType>("auth_identity");
    mapClass<AuthInfo::AuthTokenType>("auth_token");
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
        if (IpBan::am_i_banned()) {
            admin_log("Register from banned IP " +
                      ip_a(wApp->environment().clientAddress()), user);
        } else {
            admin_log("New user from IP " +
                      ip_a(wApp->environment().clientAddress()), user);
        }
    }
    return user;
}

}

