/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_SERVER_HPP_
#define THECHESS_SERVER_HPP_

#include <Wt/WServer>
#include <Wt/Dbo/FixedSqlConnectionPool>
#include <Wt/Auth/AuthService>
#include <Wt/Auth/PasswordService>

#include "Options.hpp"
#include "Session.hpp"
#include "Notifier.hpp"
#include "TaskTracker.hpp"
#include "widgets/PgnResource.hpp"

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
    Server(int argc, char** argv);
    const Options& options() const;
    dbo::FixedSqlConnectionPool& pool();
    Notifier& notifier();
    TaskTracker& tracker();

    const Wt::Auth::AuthService& auth_service() const {
        return auth_service_;
    }

    const Wt::Auth::PasswordService& password_service() const {
        return password_service_;
    }

private:
    Options options_;
    dbo::FixedSqlConnectionPool pool_;
    Notifier notifier_;
    TaskTracker tracker_;
    PgnResource pgn_;
    Wt::Auth::AuthService auth_service_;
    Wt::Auth::PasswordService password_service_;

    void auth_init();
};

}

#endif

