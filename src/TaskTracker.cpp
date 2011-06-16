#include <cstdio>
#include <map>
#include <list>
#include <utility>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>

#include <Wt/WDateTime>
#include <Wt/Dbo/Exception>

#include "TaskTracker.hpp"
#include "time_intervals.hpp"
#include "ThechessApplication.hpp"
#include "ThechessSession.hpp"
#include "ThechessServer.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"
#include "time_intervals.hpp"
#include "config.hpp"

namespace thechess {
using namespace model;

TaskTracker::TaskTracker(ThechessServer& server):
server_(server), session_(server.pool()), timer_(io_),
dummy_timer_(io_, config::tracker::dummy_timer_expiry_time)
{
    dummy_timer_.async_wait(boost::bind(&TaskTracker::check_, this, _1));
    boost::thread(&TaskTracker::io_run_, this);
    refresh_();
}

TaskTracker::~TaskTracker()
{
    timer_.cancel();
    dummy_timer_.cancel();
}

void TaskTracker::io_run_()
{
    io_.run();
}

void TaskTracker::add_or_update_task(const Object& object)
{
    mutex_.lock();
    add_or_update_task_(object);
    std::cout << "add_or_update_task() ok" <<std::endl;
    refresh_();
    mutex_.unlock();
}

void TaskTracker::check_(const boost::system::error_code& error)
{
    std::cout << "TaskTracker::check_()" <<std::endl;
    if (!error)
    {
        std::cout << "TaskTracker::check_() ok" <<std::endl;
        mutex_.lock();
        Wt::WDateTime cached_now = now();
        Objects objects;
        while (!w2t.empty())
        {
            W2T_It w2t_it = w2t.begin();
            const Wt::WDateTime& time = w2t_it->first;
            if (cached_now >= time)
            {
                const Object& object = w2t_it->second;
                try
                {
                    dbo::Transaction t(session_);
                    Wt::WDateTime new_time = object.process(objects, session_);
                    t.commit();
                    server_.notifier().emit(object);
                    if (new_time.isValid() && new_time > cached_now)
                    {
                        // update
                        t2i[object] = w2t.insert(std::make_pair(new_time, object));
                    }
                    else
                    {
                        // delete
                        t2i.erase(object);
                    }
                }
                catch (dbo::StaleObjectException)
                {
                    // reread
                    object.reread(session_);
                    // update
                    Wt::WDateTime new_time = cached_now +
                        config::tracker::stale_object_delay;
                    t2i[object] = w2t.insert(std::make_pair(new_time, object));
                }
                catch (...) // dbo::ObjectNotFoundException
                {
                    // delete
                    t2i.erase(object);
                }
                w2t.erase(w2t_it);
            }
            else
            {
                break;
            }
        }
        BOOST_FOREACH(Object object, objects)
            add_or_update_task_(object);
        refresh_();
        mutex_.unlock();
    }
}

void TaskTracker::add_or_update_task_(const Object& object)
{
    T2I_It t2i_it = t2i.find(object);
    if (t2i_it != t2i.end())
    {
        // delete old
        W2T_It w2t_it = t2i_it->second;
        w2t.erase(w2t_it);
    }
    t2i[object] = w2t.insert(std::make_pair(now(), object));
}

void TaskTracker::refresh_()
{
    if (!w2t.empty())
    {
        Wt::WDateTime next_check = std::max(w2t.begin()->first, now()+second);
        timer_.expires_at(next_check.toPosixTime()); // internally calls cancel
        timer_.async_wait(boost::bind(&TaskTracker::check_, this, _1));
    }
}

}

