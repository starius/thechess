/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_THECHESS_SESSION_H_
#define THECHESS_THECHESS_SESSION_H_

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/WSignal>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/FixedSqlConnectionPool>

#include "Options.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class TaskTracker;

class Session : public dbo::Session {
public:
    Session(dbo::FixedSqlConnectionPool& pool);
    void reconsider(TaskTracker& tracker);
    static dbo::SqlConnection* new_connection(const Options& options);
};

}

#endif
