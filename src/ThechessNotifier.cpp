
#include <boost/foreach.hpp>

#include "ThechessNotifier.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
using namespace model;

ThechessNotifier::ThechessNotifier(ThechessServer& server):
server_(server)
{
}

void ThechessNotifier::start_listenning(const Object& object)
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

void ThechessNotifier::stop_listenning(const Object& object)
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

void ThechessNotifier::emit(const ThechessEvent& event, const std::string& this_app)
{
    if (event.object_type != NoEvent &&
        (event.move != chess::move_null || event.raw_event() != 0))
    {
        mutex_.lock();
        O2I::iterator it = object2ids_.find(static_cast<Object>(event));
        if (it != object2ids_.end())
        {
            IdSet* id_set = it->second;
            BOOST_FOREACH(const std::string& id, *id_set)
            {
                if (id != this_app)
                {
                    server_.post(id, boost::bind(
                        &ThechessApplication::thechess_notify, event));
                }
            }
        }
        mutex_.unlock();
    }
}

void ThechessNotifier::app_emit(const ThechessEvent& event)
{
    if (event.object_type != NoEvent)
    {
        tApp->server().notifier().emit(event, tApp->sessionId());
        tApp->thechess_notify(event);
    }
}

}

