/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

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
    int connections_in_pool() const;

private:
    const Wt::WServer& server_;

    DatabaseType database_type_;
    std::string database_value_;
    int connections_in_pool_;

    bool read_int_value(const std::string& name, int& value);
};

}

#endif

