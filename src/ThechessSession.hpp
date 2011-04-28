#ifndef THECHESS_THECHESS_SESSION_H_
#define THECHESS_THECHESS_SESSION_H_

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/WSignal>
#include <Wt/Dbo/SqlConnection>

#include "ThechessOptions.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class ThechessSession : public dbo::Session
{
public:
    ThechessSession(const ThechessOptions& options);
    virtual ~ThechessSession();
    void reconsider();
private:
    dbo::SqlConnection* connection_;
};

}

#endif // THECHESS_THECHESS_SESSION_H_

