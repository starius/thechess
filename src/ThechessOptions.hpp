#ifndef THECHESS_OPTIONS_HPP_
#define THECHESS_OPTIONS_HPP_

#include <istream>
#include <string>

namespace thechess {

class ThechessOptions
{
public:
    enum DatabaseType
    {
        Postgres,
        Sqlite3
    };
    ThechessOptions(std::istream& ini_file);
    DatabaseType database_type() const;
    const std::string& database_value() const;

private:
    DatabaseType database_type_;
    std::string database_value_;
};

}

#endif

