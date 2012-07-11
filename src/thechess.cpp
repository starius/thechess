/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WServer>
#include <Wt/WIOService>

#include "Server.hpp"
#include "tests.hpp"

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
        server.ioService().boost::asio::io_service::stop();
        // FIXME http://redmine.emweb.be/issues/1122
        server.stop();
    }
}

