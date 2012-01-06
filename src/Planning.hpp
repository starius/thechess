/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_PLANNING_HPP_
#define THECHESS_PLANNING_HPP_

#include <Wt/Wc/Planning.hpp>

#include "thechess-global.hpp"

namespace thechess {

/** Part of server, scheduling checks of database objects.

\ingroup server
\ingroup model
*/
class Planning : public Wt::Wc::notify::PlanningServer {
public:
    /** Constructor */
    Planning(Server& server);

    /** Destructor.
    Kill all timers immediately.
    */
    ~Planning();

    /** Get server */
    Server& server() {
        return server_;
    }

private:
    Server& server_;
};

}

#endif

