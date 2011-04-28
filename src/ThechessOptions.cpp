
#include <string>
#include <set>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

#include "ThechessOptions.hpp"
#include "config.hpp"

namespace pod = boost::program_options::detail;

namespace thechess
{

ThechessOptions::ThechessOptions(std::istream& ini_file):
database_type_(config::defaults::database_type),
database_value_(config::defaults::database_value)
{
    std::set<std::string> options;
    options.insert("*");
    for (pod::config_file_iterator i(ini_file, options), e; i != e; ++i)
    {
        if (i->string_key == "database_type")
        {
            std::string database_type_str = i->value[0];
            BOOST_ASSERT(database_type_str == "postgres" ||
                database_type_str == "sqlite3");
            if (database_type_str == "postgres")
            {
                database_type_ = Postgres;
            }
            else if (database_type_str == "sqlite3")
            {
                database_type_ = Sqlite3;
            }
        }
        if (i->string_key == "database_value")
        {
            database_value_ = i->value[0];
        }
    }
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

