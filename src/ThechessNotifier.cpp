
#include <boost/foreach.hpp>

#include "ThechessNotifier.hpp"
#include "ThechessApplication.hpp"

namespace thechess {

ThechessEvent::ThechessEvent(model::ObjectType ot, int i,
    model::Game::Event ge, chess::Move m) :
model::Object(ot, i), game_event(ge), move(m)
{
}

ThechessNotifier::ThechessNotifier(ThechessServer& server):
server_(server)
{
}

void ThechessNotifier::start_listenning(const model::Object& object)
{
    mutex_.lock();
    if (object2ids_.find(object) == object2ids_.end())
    {
        object2ids_[object] = new IdSet();
    }
    IdSet* id_set = object2ids_[object];
    id_set->insert(tApp->sessionId());
    mutex_.unlock();
}

void ThechessNotifier::stop_listenning(const model::Object& object)
{
    mutex_.lock();
    if (object2ids_.find(object) == object2ids_.end())
    {
        object2ids_[object] = new IdSet();
    }
    IdSet* id_set = object2ids_[object];
    id_set->erase(tApp->sessionId());
    if (id_set->empty())
    {
        object2ids_.erase(object);
        delete id_set;
    }
    mutex_.unlock();
}

void ThechessNotifier::emit(const ThechessEvent event)
{
    mutex_.lock();
    IdSet* id_set = object2ids_[static_cast<model::Object>(event)];
    BOOST_FOREACH(const std::string& id, *id_set)
    {
        server_.post(id, boost::bind(&ThechessApplication::thechess_notify, event));
    }
    mutex_.unlock();
}

}

