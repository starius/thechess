/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_ALL_SESSIONS_HPP_
#define THECHESS_WIDGETS_ALL_SESSIONS_HPP_

#include <Wt/WGlobal>
#include <Wt/WTable>

#include "model/all.hpp"

namespace thechess {

/** List of all sessions.
Current user must have REGISTRATION_BANNER.
*/
class AllSessions : public Wt::WTable {
public:
    /** Constructor */
    AllSessions();
};

}

#endif

