#ifndef THECHESS_HPP_
#define THECHESS_HPP_

#include <fstream>

#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/WServer>

#include "ThechessApplication.hpp"
#include "ThechessOptions.hpp"
#include "model/CookieSession.hpp"
#include "tests.hpp"
#include "config.hpp"
#include "utils.hpp"

namespace c = thechess::config;

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
    server.setServerConfiguration(argc, argv);
    server.addEntryPoint(Wt::Application, createApplication, "");

    thechess::ThechessOptions options(server);
    thechess::ThechessSession session(options);
    thechess::ThechessApplication::set_session_(session);
    session.reconsider();

    if (server.start())
    {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}


#endif
