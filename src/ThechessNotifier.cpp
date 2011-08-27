/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include "ThechessNotifier.hpp"
#include "ThechessApplication.hpp"

namespace thechess {

ThechessNotifier::ThechessNotifier(ThechessServer& server):
    server_(server) {
}

void ThechessNotifier::start_listenning(const Object& object) {
    mutex_.lock();
    if (object2ids_.find(object) == object2ids_.end()) {
        object2ids_[object] = new IdSet();
    }
    IdSet* id_set = object2ids_[object];
    id_set->insert(tApp->sessionId());
    mutex_.unlock();
}

void ThechessNotifier::stop_listenning(const Object& object) {
    mutex_.lock();
    if (object2ids_.find(object) == object2ids_.end()) {
        object2ids_[object] = new IdSet();
    }
    IdSet* id_set = object2ids_[object];
    id_set->erase(tApp->sessionId());
    if (id_set->empty()) {
        object2ids_.erase(object);
        delete id_set;
    }
    mutex_.unlock();
}

void ThechessNotifier::emit(const Object& object, const std::string& this_app) {
    if (object.type != NOEVENT) {
        mutex_.lock();
        O2I::iterator it = object2ids_.find(object);
        if (it != object2ids_.end()) {
            IdSet* id_set = it->second;
            BOOST_FOREACH (const std::string& id, *id_set) {
                if (id != this_app) {
                    server_.post(id, boost::bind(
                                     &ThechessApplication::thechess_notify, object));
                }
            }
        }
        mutex_.unlock();
    }
}

void ThechessNotifier::app_emit(const Object& object) {
    if (object.type != NOEVENT) {
        tApp->server().notifier().emit(object, tApp->sessionId());
        tApp->thechess_notify(object);
    }
}

}

