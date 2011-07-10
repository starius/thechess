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

#include "ThechessOptions.hpp"
#include "config.hpp"

namespace thechess
{

ThechessOptions::ThechessOptions(const Wt::WServer& server):
server_(server),
database_type_(config::defaults::database_type),
database_value_(config::defaults::database_value),
connections_in_pool_(config::defaults::connections_in_pool)
{
    std::string value;
    if (server.readConfigurationProperty("database_type", value))
    {
        BOOST_ASSERT(value == "postgres" ||
            value == "sqlite3");
        if (value == "postgres")
        {
            database_type_ = Postgres;
        }
        else if (value == "sqlite3")
        {
            database_type_ = Sqlite3;
        }
    }

    server.readConfigurationProperty("database_value", database_value_);
    read_int_value("connections_in_pool", connections_in_pool_);
}

ThechessOptions::DatabaseType ThechessOptions::database_type() const
{
    return database_type_;
}

const std::string& ThechessOptions::database_value() const
{
    return database_value_;
}

int ThechessOptions::connections_in_pool() const
{
    return connections_in_pool_;
}

bool ThechessOptions::read_int_value(const std::string& name, int& value)
{
    std::string value_str;
    bool result = server_.readConfigurationProperty(name, value_str);
    if (result)
    {
        value = atoi(value_str.c_str());
    }
    return result;
}

}

