/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_SERVER_HPP_
#define THECHESS_SERVER_HPP_

#include <Wt/WServer>
#include <Wt/WFileResource>
#include <Wt/Dbo/FixedSqlConnectionPool>
#include <Wt/Auth/AuthService>
#include <Wt/Auth/PasswordService>

#include "notify.hpp"
#include "Options.hpp"
#include "Session.hpp"
#include "Planning.hpp"
#include "widgets/game/PgnResource.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

/** \defgroup server Server and applications
Core of thechess server.
*/

/** Descendant of Wt::WServer.
\ingroup server
*/
class Server : public Wt::WServer {
public:
    /** Constructor.
    Arguments are passed to WServer::setServerConfiguration().
    */
    Server(int argc, char** argv);

    /** Get application options */
    const Options& options() const;

    /** Get sql connection pool */
    dbo::FixedSqlConnectionPool& pool();

    /** Get notification server */
    Notifier& notifier();

    /** Get planning server */
    Planning& planning() {
        return planning_;
    }

    /** Get authentication service */
    const Wt::Auth::AuthService& auth_service() const {
        return auth_service_;
    }

    /** Get password service */
    const Wt::Auth::PasswordService& password_service() const {
        return password_service_;
    }

    /** Reads a configuration property.
    The convenience method for readConfigurationProperty().
    */
    std::string config_prop(const std::string& name) const;

private:
    Options options_;
    dbo::FixedSqlConnectionPool pool_;
    Notifier notifier_;
    Planning planning_;
    PgnResource pgn_;
    Wt::WFileResource swfstore_;
    Wt::WFileResource storage_whitelist_; // 404 Not Found
    Wt::Auth::AuthService auth_service_;
    Wt::Auth::PasswordService password_service_;

    void auth_init();
};

}

#endif

