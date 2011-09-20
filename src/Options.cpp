/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
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
    connections_in_pool_(config::defaults::CONNECTIONS_IN_POOL) {
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
}

Options::DatabaseType Options::database_type() const {
    return database_type_;
}

const std::string& Options::database_value() const {
    return database_value_;
}

int Options::connections_in_pool() const {
    return connections_in_pool_;
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

