#ifndef THECHESS_TASKTRACKER_HPP_
#define THECHESS_TASKTRACKER_HPP_

#include <boost/thread/mutex.hpp>
#include <boost/asio.hpp>

#include <Wt/WDateTime>

#include "model/Object.hpp"
#include "ThechessSession.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

class ThechessServer;

class TaskTracker
{
public:
    TaskTracker(ThechessServer& server);
    ~TaskTracker();
    void add_or_update_task(const model::Object& object);

private:
    typedef std::multimap<Wt::WDateTime, model::Object> W2T;
    typedef W2T::iterator W2T_It;
    typedef std::map<model::Object, W2T_It> T2I;
    typedef T2I::iterator T2I_It;

    W2T w2t;
    T2I t2i;
    ThechessServer& server_;
    ThechessSession session_;
    boost::mutex mutex_;
    boost::asio::io_service io_;
    boost::asio::deadline_timer timer_;
    boost::asio::deadline_timer dummy_timer_; // to avoid io_'s stopping

    void io_run_();
    void check_(const boost::system::error_code& error);
    void refresh_();
};


}

#endif

