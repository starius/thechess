/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_OPTIONS_HPP_
#define THECHESS_OPTIONS_HPP_

#include <string>

#include <Wt/WServer>

namespace thechess {

/** Settings of thechess program.
The configuration is read from wt_config.xml (Runtime Properties section).

\ingroup server
*/
class Options {
public:

    /** Wt::Dbo backend type */
    enum DatabaseType {
        POSTGRES, /**< Postgres */
        SQLITE3   /**< Sqlite3 */
    };

    /** Constructor */
    Options(const Wt::WServer& server);

    /** Get Wt::Dbo backend type */
    DatabaseType database_type() const;

    /** Get Wt::Dbo backend configuration */
    const std::string& database_value() const;

    /** Get The number of database connections in a pool */
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

