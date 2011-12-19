/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_THECHESS_SESSION_H_
#define THECHESS_THECHESS_SESSION_H_

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/WSignal>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/FixedSqlConnectionPool>
#include <Wt/Auth/Login>

#include "model/global.hpp"
#include "Options.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class TaskTracker;

/** Descendant of dbo::Session
\ingroup server
\ingroup model
*/
class Session : public dbo::Session {
public:
    Session(dbo::FixedSqlConnectionPool& pool);
    void reconsider(TaskTracker& tracker);
    static dbo::SqlConnection* new_connection(const Options& options);

    /** Access user database */
    UserDatabase& user_database() {
        return user_database_;
    }

    /** Current login state */
    Wt::Auth::Login& login() {
        return login_;
    }

private:
    UserDatabase user_database_;
    Wt::Auth::Login login_;
};

}

#endif

