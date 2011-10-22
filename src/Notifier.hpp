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

#include "model/global.hpp"

namespace thechess {

class Server;

/** Part of server, passing thechess notifications to Notifiable objects.
\ingroup server
*/
class Notifier {
public:
    /** Constructor */
    Notifier(Server& server);

    /** This method is called when constructing Notifiable */
    void start_listenning(const Object& object); // by Application

    /** This method is called when destructing Notifiable */
    void stop_listenning(const Object& object); // by Application

    /** Notify all \ref Notifiable "notibiables" listening to object updates.
    This method should be runned only by TaskTracker.
    \attention Call this method after successful transaction commiting.
    \todo hide this_app argument
    */
    void emit(const Object& object,
              const std::string& this_app = "");

    /** \copybrief emit
    This method should be runned only from Application
    (i.e., when Application::instance() is determined).
    \attention Call this method after successful transaction commiting.
    */
    static void app_emit(const Object& object); // from WApplication

private:
    Server& server_;
    typedef std::set<std::string> IdSet;
    typedef std::map<Object, IdSet*> O2I;
    O2I object2ids_;
    boost::mutex mutex_;
};

}

#endif

