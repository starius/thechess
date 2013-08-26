/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstdlib>
#include <algorithm>
#include <string>
#include <set>
#include <boost/assert.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <Wt/Wc/util.hpp>

#include "Options.hpp"
#include "Server.hpp"
#include "config.hpp"

namespace thechess {

Options::Options(const Wt::WServer& server):
    server_(&server),
    database_type_(config::defaults::DATABASE_TYPE),
    database_value_(config::defaults::DATABASE_VALUE),
    connections_in_pool_(config::defaults::CONNECTIONS_IN_POOL),
    regular_user_rights_(config::defaults::REGULAR_USER_RIGHTS),
    banned_ip_user_rights_(config::defaults::BANNED_IP_USER_RIGHTS),
    anonymous_rights_(config::defaults::ANONYMOUS_RIGHTS),
    vacation_rights_(config::defaults::VACATION_RIGHTS),
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
    max_sessions_(config::defaults::MAX_SESSIONS),
    whitelist_max_sessions_(config::defaults::WHITELIST_MAX_SESSIONS),
    time_diff_(config::defaults::TIME_DIFF) {
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
    read_int_value("vacation_rights", (int&)(vacation_rights_));
    read_locales("main_page_content_id", main_page_content_id_,
                 main_page_content_);
    read_locales("footer_content_id", footer_content_id_, footer_content_);
    read_int_value("top_logged_in_content_id", top_logged_in_content_id_);
    read_seconds("away_timeout_seconds", away_timeout_);
    read_int_value("default_settings", (int&)(default_settings_));
    server.readConfigurationProperty("pgn_site", pgn_site_);
    read_int_value("champion_id", champion_id_);
    read_int_value("best_players_shown", best_players_shown_);
    read_int_value("user_agreement_id", user_agreement_id_);
    read_seconds("game_max_preactive_seconds", game_max_preactive_);
    read_int_value("min_first_draw", min_first_draw_);
    read_int_value("max_sessions", max_sessions_);
    read_int_value("whitelist_max_sessions", whitelist_max_sessions_);
    std::string whitelist_str(config::defaults::WHITELIST);
    server.readConfigurationProperty("whitelist", whitelist_str);
    using namespace boost::algorithm;
    split(whitelist_, whitelist_str, is_any_of(" "));
    std::sort(whitelist_.begin(), whitelist_.end());
    std::vector<std::string>(whitelist_).swap(whitelist_); // reduce capacity
    read_seconds("time_diff", time_diff_);
    server_ = 0; // should not be used latter
}

int Options::main_page_content_id() const {
    return locales_id(main_page_content_, main_page_content_id_);
}

int Options::footer_content_id() const {
    return locales_id(footer_content_, footer_content_id_);
}

bool Options::ip_in_whitelist(const std::string& ip) const {
    return std::binary_search(whitelist_.begin(), whitelist_.end(), ip);
}

Options* Options::instance() {
    return &Server::instance()->options();
}

bool Options::read_int_value(const std::string& name, int& value) {
    std::string value_str;
    bool result = server_->readConfigurationProperty(name, value_str);
    if (result) {
        value = atoi(value_str.c_str());
    }
    return result;
}

bool Options::read_seconds(const std::string& name, Td& value) {
    int seconds = -1;
    if (read_int_value(name, seconds)) {
        value = seconds * SECOND;
        return true;
    }
    return false;
}

bool Options::read_locales(const std::string& name, int& main,
                           Locale2Int& locales) {
    locales.clear();
    std::string value_str;
    bool result = server_->readConfigurationProperty(name, value_str);
    if (!server_->readConfigurationProperty(name, value_str)) {
        return false;
    }
    using namespace boost::algorithm;
    std::vector<std::string> parts;
    split(parts, value_str, is_any_of(" "));
    main = atoi(parts[0].c_str());
    for (int i = 1; i < parts.size(); i++) {
        std::vector<std::string> locale_and_id;
        split(locale_and_id, parts[i], is_any_of("="));
        locales[locale_and_id[0]] = atoi(locale_and_id[1].c_str());
    }
    return true;
}

int Options::locales_id(const Locale2Int& locales, int d) const {
    if (wApp) {
        std::string main_locale = Wt::Wc::get_locale(wApp).substr(0, 2);
        Locale2Int::const_iterator it = locales.find(main_locale);
        if (it != locales.end()) {
            return it->second;
        }
    }
    return d;
}

}

