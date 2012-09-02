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
#include "config.hpp"

namespace thechess {

Options::Options(const Wt::WServer& server):
    server_(server),
    database_type_(config::defaults::DATABASE_TYPE),
    database_value_(config::defaults::DATABASE_VALUE),
    connections_in_pool_(config::defaults::CONNECTIONS_IN_POOL),
    regular_user_rights_(REGULAR_USER),
    banned_ip_user_rights_(NONE),
    anonymous_rights_(REGULAR_USER),
    main_page_content_id_(-1),
    footer_content_id_(-1),
    top_logged_in_content_id_(-1) {
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

