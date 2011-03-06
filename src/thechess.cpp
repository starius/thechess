#ifndef THECHESS_HPP_
#define THECHESS_HPP_

#include <stdlib.h>

#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/WServer>

#include "ThechessApplication.hpp"
#include "model/CookieSession.hpp"
#include "tests.hpp"

Wt::WApplication* createApplication(const Wt::WEnvironment& env)
{
    return new thechess::ThechessApplication(env);
}

int main(int argc, char **argv)
{
    #ifdef RUN_TESTS
    thechess::chess::run_tests();
    #endif // RUN_TESTS

    Wt::WServer server(argv[0]);

    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);

    thechess::ThechessApplication::consider_db();

    server.addEntryPoint(Wt::Application, createApplication, "/");

    if (server.start())
    {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}


#endif
