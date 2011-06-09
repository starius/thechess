#ifndef THECHESS_NOTIFIER_HPP_
#define THECHESS_NOTIFIER_HPP_

#include <map>
#include <set>
#include <boost/thread/mutex.hpp>

#include "model/Object.hpp"

namespace thechess {

class ThechessServer;

class ThechessNotifier
{
public:
    ThechessNotifier(ThechessServer& server);

    void start_listenning(const model::Object& object); // by ThechessApplication
    void stop_listenning(const model::Object& object); // by ThechessApplication

    // after successful transaction.commit()
    void emit(const model::ThechessEvent& event,
        const std::string& this_app=""); // not from WApplication
    static void app_emit(const model::ThechessEvent& event); // from WApplication

private:
    ThechessServer& server_;
    typedef std::set<std::string> IdSet;
    typedef std::map<model::Object, IdSet*> O2I;
    O2I object2ids_;
    boost::mutex mutex_;
};

}

#endif

