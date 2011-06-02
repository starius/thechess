#ifndef THECHESS_NOTIFIER_HPP_
#define THECHESS_NOTIFIER_HPP_

#include <map>
#include <set>
#include <boost/thread/mutex.hpp>

#include "model/Object.hpp"
#include "model/Game.hpp"
#include "chess/move.hpp"

namespace thechess {

class ThechessServer;

struct ThechessEvent : public model::Object
{
    model::Game::Event game_event;
    chess::Move move;
};

class ThechessNotifier
{
public:
    ThechessNotifier(ThechessServer& server);

    void start_listenning(model::Object& object); // by ThechessApplication
    void stop_listenning(model::Object& object); // by ThechessApplication

    void emit(const ThechessEvent event); // after successful transaction.commit()

private:
    ThechessServer& server_;
    typedef std::set<std::string> IdSet;
    typedef std::map<model::Object, IdSet*> O2I;
    O2I object2ids_;
    boost::mutex mutex_;
};

}

#endif

