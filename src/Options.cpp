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
    away_timeout_(config::defaults::AWAY_TIMEOUT) {
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
    int away_timeout_seconds = -1;
    read_int_value("away_timeout_seconds", away_timeout_seconds);
    if (away_timeout_seconds) {
        away_timeout_ = away_timeout_seconds * SECOND;
    }
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

