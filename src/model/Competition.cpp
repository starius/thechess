
#include "model/Competition.hpp"
#include "config.hpp"

namespace thechess {
namespace model {

void Competition::initialize_()
{
    CompetitionParameters::initialize_();
}

void Competition::check(ThechessEvent& event)
{

}

Competition* Competition::create_new()
{
    Competition* c = new Competition();
    c->initialize_();
    return c;
}

void Competition::create_competition(UserPtr user)
{
    state_ = RECRUITING;
    created_ = now();
    init_ = user;
}

bool Competition::is_member(UserPtr user) const
{
    return members_.find().where("id = ?").bind(user).resultList().size() == 1;
}

bool Competition::can_join(UserPtr user) const
{
    return state_ == RECRUITING &&
        !is_member(user) &&
        user->elo_rating() >= min_rating() &&
        user->elo_rating() <= max_rating() &&
        user->classification() >= min_classification() &&
        user->classification() <= max_classification();
}

void Competition::join(UserPtr user)
{
    if (can_join(user))
    {
        members_.insert(user);
    }
}

bool Competition::can_leave(UserPtr user) const
{
    return state_ == RECRUITING &&
        is_member(user);
}

void Competition::leave(UserPtr user)
{
    if (can_leave(user))
    {
        members_.erase(user);
    }
}

bool Competition::can_kick(UserPtr kicker, UserPtr kicked) const
{
    return state_ == RECRUITING &&
        is_member(kicked) &&
        kicker == init_;
}

void Competition::kick(UserPtr kicker, UserPtr kicked)
{
    if (can_kick(kicker, kicked))
    {
        kick(kicker, kicked);
    }
}

bool Competition::can_change_parameters(UserPtr user) const
{
    return state_ == RECRUITING &&
        user == init_;
}

bool Competition::can_cancel(UserPtr user) const
{
    return state_ == RECRUITING &&
        user == init_;
}

void Competition::cancel(UserPtr user)
{
    if (can_cancel(user))
    {
        state_ = CANCELLED;
    }
}

bool Competition::is_winner(UserPtr user) const
{
    return winners_.find().where("id = ?").bind(user).resultList().size() == 1;
}

}
}

