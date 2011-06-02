#ifndef THECHESS_HPP_
#define THECHESS_HPP_

#include <Wt/WServer>

#include "ThechessServer.hpp"
#include "tests.hpp"

using namespace thechess;

int main(int argc, char **argv)
{
    #ifdef RUN_TESTS
    thechess::chess::run_tests();
    #endif // RUN_TESTS

    ThechessServer server(argc, argv);

    if (server.start())
    {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}

#endif

