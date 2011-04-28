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
    
    std::string ini_filepath = thechess::expand_path(c::ini_filename);
    std::ifstream ini_file(ini_filepath.c_str());
    thechess::ThechessOptions options(ini_file);
    thechess::ThechessSession session(options);
    thechess::ThechessApplication::set_session_(session);
    session.reconsider();

    Wt::WServer server(argv[0]);
    server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
    server.addEntryPoint(Wt::Application, createApplication, "/");
    if (server.start())
    {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
}


#endif
