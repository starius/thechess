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
#include <Wt/Wc/util.hpp>

#include "Server.hpp"
#include "Application.hpp"
#include "utils/utils.hpp"
#include "config.hpp"

namespace thechess {

Wt::WApplication* createApplication(Server* server,
                                    const Wt::WEnvironment& env) {
    return new thechess::Application(env, *server);
}

Wt::WApplication* createWidgetSet(Server* server,
                                  const Wt::WEnvironment& env) {
    return new thechess::Application(true, env, *server);
}

Session* session_creator(Server* server) {
    return new Session(server->pool());
}

// FIXME Debian hardcode!!
const char* swfstore = "/usr/share/javascript/yui/swfstore/swfstore.swf";

Server* Server::instance_ = 0;

Server::Server(int argc, char** argv):
    Wt::WServer(argv[0], first_file(config::WT_CONFIG_FILES,
                                    config::WT_CONFIG_FILES_SIZE)),
    // options_ needs read conf
    options_((setServerConfiguration(argc, argv), *this)),
    pool_(Session::new_connection(options_), options_.connections_in_pool()),
    notifier_(this), planning_(&ioService()), pgn_(*this), all_pgn_(*this),
    swfstore_("application/x-shockwave-flash", swfstore),
    password_service_(auth_service_),
    argc_(argc), argv_(argv) {
    instance_ = this;
    notifier_.set_direct_to_this(this);
    planning_.set_delay(config::tracker::DELAY);
    planning_.set_notification_server(&notifier_);
    //addResource(&all_pgn_, "/pgn/all.pgn");
    addResource(&pgn_, "/pgn/");
    addResource(&swfstore_, "/swfstore.swf");
    addResource(&storage_whitelist_, "/storage-whitelist.xml");
    addEntryPoint(Wt::WidgetSet, boost::bind(createWidgetSet, this, _1),
                  "/thechess.js", "/favicon.ico");
    addEntryPoint(Wt::Application, boost::bind(createApplication, this, _1),
                  "", "/favicon.ico");
    auth_init();
    Session session(pool_);
    session.reconsider(*this);
}

void Server::reread_options() {
    {
        Wt::WServer server_copy(argv_[0], first_file(config::WT_CONFIG_FILES,
                                config::WT_CONFIG_FILES_SIZE));
        // FIXME races
        Wt::WServer::instance_ = this;
        server_copy.setServerConfiguration(argc_, argv_);
        // FIXME races
        Wt::WServer::instance_ = this;
        options_ = Options(server_copy);
    }
    // FIXME races
    Wt::WServer::instance_ = this;
}

void Server::auth_init() {
    using namespace Wt::Auth;
    auth_service_.setAuthTokensEnabled(true, config::COOKIE_NAME);
    auth_service_.setAuthTokenValidity(config::COOKIE_AGE.total_minutes());
    auth_service_.setEmailVerificationEnabled(true);
    password_service_.setAttemptThrottlingEnabled(true);
    PasswordStrengthValidator* s = new PasswordStrengthValidator();
    s->setMinimumPassPhraseWords(2);
    s->setMinimumLength(Wt::Auth::PasswordStrengthValidator::OneCharClass, 10);
    s->setMinimumLength(Wt::Auth::PasswordStrengthValidator::TwoCharClass, 7);
    s->setMinimumLength(Wt::Auth::PasswordStrengthValidator::PassPhrase, 6);
    s->setMinimumLength(Wt::Auth::PasswordStrengthValidator::ThreeCharClass, 5);
    s->setMinimumLength(Wt::Auth::PasswordStrengthValidator::FourCharClass, 4);
    password_service_.setStrengthValidator(s);
    PasswordVerifier* verifier = new PasswordVerifier();
    verifier->addHashFunction(new BCryptHashFunction(7));
    password_service_.setVerifier(verifier);
}

}

