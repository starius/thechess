#ifndef THECHESS_THECHESS_SESSION_H_
#define THECHESS_THECHESS_SESSION_H_

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/WSignal>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/FixedSqlConnectionPool>

#include "ThechessOptions.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class TaskTracker;

class ThechessSession : public dbo::Session
{
public:
    ThechessSession(dbo::FixedSqlConnectionPool& pool);
    void reconsider(TaskTracker& tracker);
    static dbo::SqlConnection* new_connection(const ThechessOptions& options);
};

}

#endif // THECHESS_THECHESS_SESSION_H_

