
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

bool User::can_set_classification(UserPtr user) const
{
    return user && user.id() == id();
}

void User::set_classification(UserPtr user, Classification classification)
{
    if (can_set_classification(user))
    {
        classification_ = classification;
        classification_confirmer_.reset();
    }
}

Classification User::classification() const
{
    return classification_;
}

Wt::WString User::classification_str() const
{
    return classification2str(classification());
}

bool User::can_confirm_classification(UserPtr user) const
{
    return user && user->rights() >= moderator;
}

void User::confirm_classification(UserPtr user)
{
    if (can_confirm_classification(user))
    {
        classification_confirmer_ = user;
    }
}

bool User::classification_confirmed() const
{
    return classification_confirmer_;
}

UserPtr User::classification_confirmer() const
{
    return classification_confirmer_;
}

}
}
