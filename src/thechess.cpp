/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include <Wt/WServer>

#include "ThechessServer.hpp"
#include "tests.hpp"

using namespace thechess;

int main(int argc, char **argv) {
#ifdef RUN_TESTS
    thechess::chess::run_tests();
#endif // RUN_TESTS

    ThechessServer server(argc, argv);

    if (server.start()) {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}

