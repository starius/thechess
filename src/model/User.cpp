
#include "model/User.hpp"

namespace thechess {
namespace model {

void User::set_password(const Wt::WString& password)
{
    password_ = password;
}

bool User::test_password(const Wt::WString& password) const
{
    return password_ == password;
}


dbo::Query<GamePtr> User::games() const
{
    return session()->find<Game>().where("white_id = ? or black_id = ?")
        .bind(id()).bind(id());
}

}
}
