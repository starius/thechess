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
#include <Wt/Auth/PasswordVerifier>
#include <Wt/Auth/PasswordStrengthValidator>
#include <Wt/Auth/HashFunction>

#include "Server.hpp"
#include "Application.hpp"
#include "utils/utils.hpp"
#include "config.hpp"

namespace thechess {

Wt::WApplication* createApplication(Server* server, const Wt::WEnvironment& env) {
    return new thechess::Application(env, *server);
}

Session* session_creator(Server* server) {
    return new Session(server->pool());
}

Server::Server(int argc, char** argv):
    Wt::WServer(argv[0], first_file(config::WT_CONFIG_FILES, config::WT_CONFIG_FILES_SIZE)),
    options_((setServerConfiguration(argc, argv), *this)), // options_ needs read conf
    pool_(Session::new_connection(options_), options_.connections_in_pool()),
    notifier_(this), planning_(*this), pgn_(*this),
    password_service_(auth_service_) {
    addResource(&pgn_, "/pgn/");
    addEntryPoint(Wt::Application, boost::bind(createApplication, this, _1), "", "/favicon.ico");
    auth_init();
    planning_.start();
    Session session(pool_);
    session.reconsider(*this);
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

void Server::auth_init() {
    using namespace Wt::Auth;
    auth_service_.setAuthTokensEnabled(true, config::COOKIE_NAME);
    auth_service_.setEmailVerificationEnabled(true);
    password_service_.setAttemptThrottlingEnabled(true);
    password_service_.setStrengthValidator(new PasswordStrengthValidator());
    PasswordVerifier* verifier = new PasswordVerifier();
    verifier->addHashFunction(new BCryptHashFunction(7));
    password_service_.setVerifier(verifier);
}

}

