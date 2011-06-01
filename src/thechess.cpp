#ifndef THECHESS_HPP_
#define THECHESS_HPP_

#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/WServer>

#include "ThechessApplication.hpp"
#include "ThechessOptions.hpp"
#include "model/CookieSession.hpp"
#include "tests.hpp"
#include "config.hpp"
#include "utils.hpp"
#include "TaskTracker.hpp"

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

    std::string wt_config = thechess::first_file(c::wt_config_files,
        c::wt_config_files_size);
    if (wt_config.empty())
    {
        std::cerr << "Can not find wt_config.xml" << std::endl;
        exit(1);
    }

    Wt::WServer server(argv[0], wt_config.c_str());
    server.setServerConfiguration(argc, argv);
    server.addEntryPoint(Wt::Application, createApplication, "");

    thechess::ThechessOptions options(server);
    thechess::ThechessSession::set_options(&options);
    thechess::ThechessSession* session = new thechess::ThechessSession();
    session->reconsider();

    if (server.start())
    {
        Wt::WServer::waitForShutdown();
        server.stop();
    }
    delete session;
}


#endif
