
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

namespace c = config;

Wt::WApplication* createApplication(ThechessServer* server, const Wt::WEnvironment& env)
{
    return new thechess::ThechessApplication(env, *server);
}

ThechessServer::ThechessServer(int argc, char **argv):
Wt::WServer(argv[0], first_file(c::wt_config_files, c::wt_config_files_size)),
options_((setServerConfiguration(argc, argv), *this)), // options_ needs read conf
pool_(ThechessSession::new_connection(options_), options_.connections_in_pool()),
notifier_(*this), tracker_(*this)
{
    addEntryPoint(Wt::Application, boost::bind(createApplication, this, _1), "");
    ThechessSession session(pool_);
    session.reconsider(tracker_);
}

const ThechessOptions& ThechessServer::options() const
{
    return options_;
}

dbo::FixedSqlConnectionPool& ThechessServer::pool()
{
    return pool_;
}

ThechessNotifier& ThechessServer::notifier()
{
    return notifier_;
}

TaskTracker& ThechessServer::tracker()
{
    return tracker_;
}

}

