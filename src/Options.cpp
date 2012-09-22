/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <string>
#include <set>
#include <boost/assert.hpp>

#include "Options.hpp"
#include "Server.hpp"
#include "config.hpp"

namespace thechess {

Options::Options(const Wt::WServer& server):
    server_(server),
    database_type_(config::defaults::DATABASE_TYPE),
    database_value_(config::defaults::DATABASE_VALUE),
    connections_in_pool_(config::defaults::CONNECTIONS_IN_POOL),
    regular_user_rights_(config::defaults::REGULAR_USER_RIGHTS),
    banned_ip_user_rights_(config::defaults::BANNED_IP_USER_RIGHTS),
    anonymous_rights_(config::defaults::ANONYMOUS_RIGHTS),
    main_page_content_id_(config::defaults::MAIN_PAGE_CONTENT_ID),
    footer_content_id_(config::defaults::FOOTER_CONTENT_ID),
    top_logged_in_content_id_(config::defaults::TOP_LOGGED_IN_CONTENT_ID),
    away_timeout_(config::defaults::AWAY_TIMEOUT),
    default_settings_(config::defaults::DEFAULT_SETTINGS),
    pgn_site_(config::defaults::PGN_SITE),
    champion_id_(config::defaults::CHAMPION_ID),
    best_players_shown_(config::defaults::BEST_PLAYERS_SHOWN),
    user_agreement_id_(config::defaults::USER_AGREEMENT_ID),
    game_max_preactive_(config::defaults::GAME_MAX_PREACTIVE),
    min_first_draw_(config::min::FIRST_DRAW),
    max_sessions_(config::defaults::MAX_SESSIONS) {
    std::string value;
    if (server.readConfigurationProperty("database_type", value)) {
        BOOST_ASSERT(value == "postgres" ||
                     value == "sqlite3");
        if (value == "postgres") {
            database_type_ = POSTGRES;
        } else if (value == "sqlite3") {
            database_type_ = SQLITE3;
        }
    }
    server.readConfigurationProperty("database_value", database_value_);
    read_int_value("connections_in_pool", connections_in_pool_);
    read_int_value("regular_user_rights", (int&)(regular_user_rights_));
    read_int_value("banned_ip_user_rights", (int&)(banned_ip_user_rights_));
    read_int_value("anonymous_rights", (int&)(anonymous_rights_));
    read_int_value("main_page_content_id", main_page_content_id_);
    read_int_value("footer_content_id", footer_content_id_);
    read_int_value("top_logged_in_content_id", top_logged_in_content_id_);
    int seconds = -1;
    if (read_int_value("away_timeout_seconds", seconds)) {
        away_timeout_ = seconds * SECOND;
    }
    read_int_value("default_settings", (int&)(default_settings_));
    server.readConfigurationProperty("pgn_site", pgn_site_);
    read_int_value("champion_id", champion_id_);
    read_int_value("best_players_shown", best_players_shown_);
    read_int_value("user_agreement_id", user_agreement_id_);
    if (read_int_value("game_max_preactive_seconds", seconds)) {
        game_max_preactive_ = seconds * SECOND;
    }
    read_int_value("min_first_draw", min_first_draw_);
    read_int_value("max_sessions", max_sessions_);
}

Options* Options::instance() {
    return &Server::instance()->options();
}

bool Options::read_int_value(const std::string& name, int& value) {
    std::string value_str;
    bool result = server_.readConfigurationProperty(name, value_str);
    if (result) {
        value = atoi(value_str.c_str());
    }
    return result;
}

}

