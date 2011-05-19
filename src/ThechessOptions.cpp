
#include <string>
#include <set>

#include "ThechessOptions.hpp"
#include "config.hpp"

namespace thechess
{

ThechessOptions::ThechessOptions(const Wt::WServer& server):
database_type_(config::defaults::database_type),
database_value_(config::defaults::database_value)
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
}

ThechessOptions::DatabaseType ThechessOptions::database_type() const
{
    return database_type_;
}

const std::string& ThechessOptions::database_value() const
{
    return database_value_;
}

}

