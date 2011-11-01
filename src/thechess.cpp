/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WServer>

#include "Server.hpp"
#include "tests.hpp"

namespace thechess {

/** \mainpage TheChess

TheChess, chess game web application written in C++ and based on Wt
<br>

Conventions:
 - "indexes" are 0-based, "numbers" are 1-based

*/

/** \namespace thechess
The namespace for thechess
*/

}

using namespace thechess;

int main(int argc, char** argv) {
    if (argc >= 2 && std::string(argv[1]) == "--run-tests") {
        // FIXME
        thechess::run_tests();
        exit(0);
    }
    Server server(argc, argv);
    if (server.start()) {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}

