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

class ThechessSession : public dbo::Session
{
public:
    ThechessSession();
    void reconsider();
    static void set_options(const ThechessOptions* options);
private:
    static dbo::FixedSqlConnectionPool* pool_;
};

}

#endif // THECHESS_THECHESS_SESSION_H_

