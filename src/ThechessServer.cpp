/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <functional>
#include <stdexcept>
#include <boost/bind.hpp>

#include <Wt/WEnvironment>
#include <Wt/WApplication>

#include "ThechessServer.hpp"
#include "ThechessApplication.hpp"
#include "utils.hpp"
#include "config.hpp"

namespace thechess {

Wt::WApplication* createApplication(ThechessServer* server, const Wt::WEnvironment& env) {
    return new thechess::ThechessApplication(env, *server);
}

ThechessServer::ThechessServer(int argc, char** argv):
    Wt::WServer(argv[0], first_file(config::WT_CONFIG_FILES, config::WT_CONFIG_FILES_SIZE)),
    options_((setServerConfiguration(argc, argv), *this)), // options_ needs read conf
    pool_(ThechessSession::new_connection(options_), options_.connections_in_pool()),
    notifier_(*this), tracker_(*this), pgn_(*this) {
    addResource(&pgn_, "/pgn/");
    addEntryPoint(Wt::Application, boost::bind(createApplication, this, _1), "", "/favicon.ico");
    ThechessSession session(pool_);
    session.reconsider(tracker_);
}

const ThechessOptions& ThechessServer::options() const {
    return options_;
}

dbo::FixedSqlConnectionPool& ThechessServer::pool() {
    return pool_;
}

ThechessNotifier& ThechessServer::notifier() {
    return notifier_;
}

TaskTracker& ThechessServer::tracker() {
    return tracker_;
}

}

