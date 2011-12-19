/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <functional>
#include <stdexcept>
#include <boost/bind.hpp>

#include <Wt/WEnvironment>
#include <Wt/WApplication>

#include "Server.hpp"
#include "Application.hpp"
#include "utils/utils.hpp"
#include "config.hpp"

namespace thechess {

Wt::WApplication* createApplication(Server* server, const Wt::WEnvironment& env) {
    return new thechess::Application(env, *server);
}

Server::Server(int argc, char** argv):
    Wt::WServer(argv[0], first_file(config::WT_CONFIG_FILES, config::WT_CONFIG_FILES_SIZE)),
    options_((setServerConfiguration(argc, argv), *this)), // options_ needs read conf
    pool_(Session::new_connection(options_), options_.connections_in_pool()),
    notifier_(*this), tracker_(*this), pgn_(*this) {
    addResource(&pgn_, "/pgn/");
    addEntryPoint(Wt::Application, boost::bind(createApplication, this, _1), "", "/favicon.ico");
    Session session(pool_);
    session.reconsider(tracker_);
}

const Options& Server::options() const {
    return options_;
}

dbo::FixedSqlConnectionPool& Server::pool() {
    return pool_;
}

Notifier& Server::notifier() {
    return notifier_;
}

TaskTracker& Server::tracker() {
    return tracker_;
}

}

