#ifndef THECHESS_OPTIONS_HPP_
#define THECHESS_OPTIONS_HPP_

#include <string>

#include <Wt/WServer>

namespace thechess {

class ThechessOptions
{
public:
    enum DatabaseType
    {
        Postgres,
        Sqlite3
    };
    ThechessOptions(const Wt::WServer& server);
    DatabaseType database_type() const;
    const std::string& database_value() const;

private:
    DatabaseType database_type_;
    std::string database_value_;
};

}

#endif

