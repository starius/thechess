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

#include "ThechessOptions.hpp"
#include "ThechessSession.hpp"
#include "ThechessNotifier.hpp"
#include "TaskTracker.hpp"
#include "widgets/PgnResource.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class ThechessServer : public Wt::WServer {
public:
    ThechessServer(int argc, char** argv);
    const ThechessOptions& options() const;
    dbo::FixedSqlConnectionPool& pool();
    ThechessNotifier& notifier();
    TaskTracker& tracker();

private:
    ThechessOptions options_;
    dbo::FixedSqlConnectionPool pool_;
    ThechessNotifier notifier_;
    TaskTracker tracker_;
    widgets::PgnResource pgn_;
};

}

#endif

