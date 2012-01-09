/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "Planning.hpp"
#include "Server.hpp"
#include "config.hpp"

namespace thechess {

Planning::Planning(Server& server):
    Wt::Wc::notify::PlanningServer(&server.ioService()),
    server_(server) {
    set_delay(config::tracker::DELAY);
    set_notification_server(&server.notifier());
}

}

