#ifndef THECHESS_MODEL_USER_H_
#define THECHESS_MODEL_USER_H_

#include <vector>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/ptr>
#include <Wt/WString>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Dbo/collection>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/Session>
namespace dbo = Wt::Dbo;

namespace thechess {
namespace model {
    class User;
    typedef dbo::ptr<User> UserPtr;
    typedef dbo::collection<UserPtr> Users;
}
}

#include "model/Game.hpp"
#include "model/EloPlayer.hpp"
#include "model/CookieSession.hpp"
#include "model/UserClassification.hpp"

namespace thechess {
namespace model {

class User : public model::EloPlayer, public dbo::Dbo<User>
{
public:
    enum Rights {
        admin = 5,
        moderator = 2,
        regular_user = 0
    };

    template<class Action>
    void persist(Action& a)
    {
        EloPlayer::persist(a);

        dbo::field(a, username_, "username");
        dbo::field(a, password_, "password");
        dbo::field(a, rights_, "rights");
        dbo::field(a, sessions_, "sessions");

        dbo::hasMany(a, white_games_, dbo::ManyToOne, "white");
        dbo::hasMany(a, black_games_, dbo::ManyToOne, "black");
        dbo::hasMany(a, black_games_, dbo::ManyToOne, "black");
    }

    void set_password(const Wt::WString& password);
    bool test_password(const Wt::WString& password) const;
    const Wt::WString& username() const { return username_; }
    void set_username(Wt::WString username) { username_ = username; }
    Rights rights() const { return rights_; }
    void set_rights(Rights rights) { rights_ = rights; }
    void login() { sessions_ += 1; }
    void logout() { sessions_ -= 1; }
    bool online() const { return sessions_ != 0; }
    dbo::Query<GamePtr> games() const;

private:
    Wt::WString username_;
    Wt::WString password_;
    Rights rights_; // default constructor: 0
    int sessions_;

    Games white_games_;
    Games black_games_;
};


}
}

#endif // THECHESS_MODEL_USER_H_
