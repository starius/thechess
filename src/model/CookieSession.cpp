
#include <Wt/WApplication>

#include "model/CookieSession.hpp"

namespace thechess {
namespace model {

CookieSession::CookieSession()
{
}

CookieSession::CookieSession(const std::string& cookie_id) :
cookie_id_(cookie_id)
{
}

}
}
