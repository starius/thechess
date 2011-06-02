#ifndef THECHESS_SERVER_HPP_
#define THECHESS_SERVER_HPP_

#include <Wt/WServer>
#include <Wt/Dbo/FixedSqlConnectionPool>

#include "ThechessOptions.hpp"
#include "ThechessSession.hpp"
#include "ThechessNotifier.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class ThechessServer : public Wt::WServer
{
public:
    ThechessServer(int argc, char **argv);
    const ThechessOptions& options() const;
    dbo::FixedSqlConnectionPool& pool();
    ThechessNotifier& notifier();

private:
    ThechessOptions options_;
    dbo::FixedSqlConnectionPool pool_;
    ThechessSession session_;
    ThechessNotifier notifier_;
};

}

#endif

