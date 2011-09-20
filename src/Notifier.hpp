/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_NOTIFIER_HPP_
#define THECHESS_NOTIFIER_HPP_

#include <map>
#include <set>
#include <boost/thread/mutex.hpp>

#include "model/Object.hpp"

namespace thechess {

class ThechessServer;

class ThechessNotifier {
public:
    ThechessNotifier(ThechessServer& server);

    void start_listenning(const Object& object); // by ThechessApplication
    void stop_listenning(const Object& object); // by ThechessApplication

    // after successful transaction.commit()
    void emit(const Object& object,
              const std::string& this_app = ""); // not from WApplication
    static void app_emit(const Object& object); // from WApplication

private:
    ThechessServer& server_;
    typedef std::set<std::string> IdSet;
    typedef std::map<Object, IdSet*> O2I;
    O2I object2ids_;
    boost::mutex mutex_;
};

}

#endif

