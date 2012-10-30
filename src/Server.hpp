/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_SERVER_HPP_
#define THECHESS_SERVER_HPP_

#include <Wt/WFileResource>
#include <Wt/Dbo/FixedSqlConnectionPool>
#include <Wt/Auth/AuthService>
#include <Wt/Auth/PasswordService>

#include "Options.hpp"
#include "SharedBoardIndex.hpp"
#include "Session.hpp"
#include "model/object.hpp"
#include "widgets/game/PgnResource.hpp"

// FIXME nasty public morozov
#define private friend class thechess::Server; private
#include <Wt/WServer>
#undef private

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
    const Options& options() const {
        return options_;
    }

    /** Get application options */
    Options& options() {
        return options_;
    }

    /** Reread options from config file */
    void reread_options();

    /** Get board index */
    SharedBoardIndex& shared_board_index() {
        return shared_board_index_;
    }

    /** Get sql connection pool */
    dbo::FixedSqlConnectionPool& pool() {
        return pool_;
    }

    /** Get authentication service */
    const Wt::Auth::AuthService& auth_service() const {
        return auth_service_;
    }

    /** Get password service */
    const Wt::Auth::PasswordService& password_service() const {
        return password_service_;
    }

    /** Return current server */
    static Server* instance() {
        return instance_;
    }

private:
    Options options_;
    SharedBoardIndex shared_board_index_;
    dbo::FixedSqlConnectionPool pool_;
    Notifier notifier_;
    Planning planning_;
    PgnResource pgn_;
    AllPgnResource all_pgn_;
    Wt::WFileResource swfstore_;
    Wt::WFileResource storage_whitelist_; // 404 Not Found
    Wt::Auth::AuthService auth_service_;
    Wt::Auth::PasswordService password_service_;
    int argc_;
    char** argv_;

    static Server* instance_;

    void auth_init();

    friend Notifier* t_notifier();
    friend Planning* t_planning();
};

}

#endif

