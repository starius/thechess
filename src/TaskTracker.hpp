/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_TASKTRACKER_HPP_
#define THECHESS_TASKTRACKER_HPP_

#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

#include <Wt/WDateTime>

#include "model/object.hpp"
#include "Session.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class Server;

/** Part of server, scheduling checks of database objects.
\ingroup server
\ingroup model
*/
class TaskTracker {
public:
    TaskTracker(Server& server);
    ~TaskTracker();
    void add_or_update_task(const Object& object);

private:
    typedef std::multimap<Wt::WDateTime, Object> W2T;
    typedef W2T::iterator W2T_It;
    typedef std::map<Object, W2T_It> T2I;
    typedef T2I::iterator T2I_It;

    W2T w2t;
    T2I t2i;
    Server& server_;
    Session session_;
    boost::mutex mutex_;
    boost::asio::io_service io_;
    boost::asio::deadline_timer timer_;
    boost::asio::deadline_timer dummy_timer_; // to avoid io_'s stopping

    void io_run_();
    void check_(const boost::system::error_code& error);
    void add_or_update_task_(const Object& object);
    void refresh_();
};

}

#endif

