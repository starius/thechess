
#include <boost/assert.hpp>

#include <Wt/Dbo/Transaction>

#include "model/Object.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"
#include "model/Competition.hpp"
#include "config.hpp"

namespace thechess {
namespace model {

Object::Object(ObjectType ot, int i) :
object_type(ot), id(i)
{
}

ThechessEvent::ThechessEvent(const Object& object):
Object(object), move(chess::move_null), event_(0)
{
}

ThechessEvent::ThechessEvent(ObjectType ot, int i):
Object(ot, i), move(chess::move_null), event_(0)
{
}

ThechessEvent::ThechessEvent(ObjectType ot, int i,
    GameEvent ge, chess::Move m) :
Object(ot, i), move(m), event_(static_cast<int>(ge))
{
}

GameEvent ThechessEvent::game_event() const
{
    BOOST_ASSERT(object_type == GameObject);
    return static_cast<GameEvent>(event_);
}

int ThechessEvent::raw_event() const
{
    return event_;
}

void ThechessEvent::set_event(GameEvent event)
{
    BOOST_ASSERT(object_type == GameObject);
    event_ = static_cast<int>(event);
}

ThechessEvent::operator bool() const
{
    return event_ != 0;
}

void Object::reread(dbo::Session& session) const
{
    dbo::Transaction t(session);
    if (object_type == GameObject)
    {
        session.load<Game>(id).reread();
    }
    if (object_type == UserObject)
    {
        session.load<User>(id).reread();
    }
    if (object_type == CompetitionObject)
    {
        session.load<Competition>(id).reread();
    }
    t.commit();
}

Wt::WDateTime Object::process(ThechessEvent& event, dbo::Session& session) const
{
    Wt::WDateTime result;
    if (object_type == GameObject)
    {
        GamePtr game = session.load<Game>(id);
        game.reread();
        game.modify()->check(event);
        result = game->next_check();
    }
    // FIXME: UserObject, CompetitionObject
    result += config::tracker::delay;
    return result;
}

}
}

