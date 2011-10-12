/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_SERVER_HPP_
#define THECHESS_SERVER_HPP_

#include <Wt/WServer>
#include <Wt/Dbo/FixedSqlConnectionPool>

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

private:
    Options options_;
    dbo::FixedSqlConnectionPool pool_;
    Notifier notifier_;
    TaskTracker tracker_;
    PgnResource pgn_;
};

}

#endif

