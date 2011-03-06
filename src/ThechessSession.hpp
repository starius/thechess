#ifndef THECHESS_THECHESS_SESSION_H_
#define THECHESS_THECHESS_SESSION_H_

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/WSignal>
#ifdef THECHESS_SQLITE
    #include <Wt/Dbo/backend/Sqlite3>
#elif THECHESS_MYSQL
    #include <Wt/Dbo/backend/MySQL.h>
#elif THECHESS_POSTGRES
    #include <Wt/Dbo/backend/Postgres>
#endif

namespace dbo = Wt::Dbo;

namespace thechess {

class ThechessSession : public dbo::Session
{
public:
    ThechessSession();
private:
#ifdef THECHESS_SQLITE
    dbo::backend::Sqlite3 connection_;
#elif THECHESS_MYSQL
    dbo::backend::MySQL connection_;
#elif THECHESS_POSTGRES
    dbo::backend::Postgres connection_;
#endif
};

}

#endif // THECHESS_THECHESS_SESSION_H_

