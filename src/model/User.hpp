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
    typedef std::vector<UserPtr> UsersVector;
}
}

#include "model/Game.hpp"
#include "model/Competition.hpp"
#include "model/EloPlayer.hpp"
#include "model/CookieSession.hpp"
#include "model/UserClassification.hpp"

namespace thechess {
namespace model {

class User : public dbo::Dbo<User>
{
public:
    enum Rights {
        admin = 5,
        moderator = 2,
        regular_user = 0
    };

    User();
    User(bool);

    template<class Action>
    void persist(Action& a)
    {
        dbo::field(a, username_, "username");
        dbo::field(a, password_, "password");
        dbo::field(a, rights_, "rights");
        dbo::field(a, sessions_, "sessions");
        dbo::field(a, classification_, "classification");
        dbo::field(a, classification_confirmer_, "classification_confirmer");
        dbo::hasMany(a, classification_confirmed_, dbo::ManyToOne, "classification_confirmer");

        dbo::hasMany(a, white_games_, dbo::ManyToOne, "white");
        dbo::hasMany(a, black_games_, dbo::ManyToOne, "black");
        dbo::hasMany(a, won_games_, dbo::ManyToOne, "winner_game");
        dbo::hasMany(a, init_games_, dbo::ManyToOne, "init_game");

        dbo::hasMany(a, competitions_, dbo::ManyToMany, "members_competitions");
        dbo::hasMany(a, init_competitions_, dbo::ManyToOne, "init_competitions");
        dbo::hasMany(a, virtual_allower_, dbo::ManyToOne, "virtual_allower");
        dbo::hasMany(a, won_competitions_, dbo::ManyToMany, "winners_competition");

        dbo::field(a, games_stat_, "games_stat");
    }

    void set_password(const std::string& password);
    bool test_password(const std::string& password) const;
    const Wt::WString& username() const { return username_; }
    void set_username(Wt::WString username) { username_ = username; }
    Rights rights() const { return rights_; }
    void set_rights(Rights rights) { rights_ = rights; }
    void login() { sessions_ += 1; }
    void logout() { sessions_ -= 1; }
    bool online() const { return sessions_ != 0; }
    dbo::Query<GamePtr> games() const;

    bool can_set_classification(UserPtr user) const;
    void set_classification(UserPtr user, Classification classification);
    Classification classification() const;
    Wt::WString classification_str() const;

    bool can_confirm_classification(UserPtr user) const;
    void confirm_classification(UserPtr user);
    bool classification_confirmed() const;
    UserPtr classification_confirmer() const;

    const EloPlayer& games_stat() const { return games_stat_; }
    EloPlayer& games_stat() { return games_stat_; }

private:
    Wt::WString username_;
    std::string password_; // UTF8
    Rights rights_; // default constructor: 0
    int sessions_;

    Games white_games_;
    Games black_games_;
    Games won_games_;
    Games init_games_;

    Classification classification_;
    UserPtr classification_confirmer_;
    Users classification_confirmed_;

    Competitions competitions_;
    Competitions init_competitions_;
    Competitions virtual_allower_;
    Competitions won_competitions_;

    EloPlayer games_stat_;
};

}
}

#endif // THECHESS_MODEL_USER_H_
