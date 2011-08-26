/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

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

TaskTracker::TaskTracker(ThechessServer& server):
    server_(server), session_(server.pool()), timer_(io_),
    dummy_timer_(io_, config::tracker::dummy_timer_expiry_time) {
    dummy_timer_.async_wait(boost::bind(&TaskTracker::check_, this, _1));
    boost::thread(&TaskTracker::io_run_, this);
    refresh_();
}

TaskTracker::~TaskTracker() {
    timer_.cancel();
    dummy_timer_.cancel();
}

void TaskTracker::io_run_() {
    io_.run();
}

void TaskTracker::add_or_update_task(const Object& object) {
    mutex_.lock();
    add_or_update_task_(object);
    std::cout << "add_or_update_task() ok" << std::endl;
    refresh_();
    mutex_.unlock();
}

void TaskTracker::check_(const boost::system::error_code& error) {
    if (!error) {
        std::cout << "TaskTracker::check_()" << std::endl;
        mutex_.lock();
        Wt::WDateTime cached_now = now();
        Objects objects;
        bool need_reread = false;
        while (!w2t.empty()) {
            W2T_It w2t_it = w2t.begin();
            const Wt::WDateTime& time = w2t_it->first;
            if (cached_now >= time) {
                const Object& object = w2t_it->second;
                try {
                    dbo::Transaction t(session_);
                    if (need_reread) {
                        session_.rereadAll();
                    }
                    need_reread = false;
                    Wt::WDateTime new_time = object.process(objects, session_);
                    t.commit();
                    std::cout << "TaskTracker::check_ commit" << std::endl;
                    server_.notifier().emit(object);
                    if (new_time.isValid() && new_time > cached_now) {
                        t2i[object] = w2t.insert(std::make_pair(new_time, object));
                    } else {
                        t2i.erase(object);
                    }
                } catch (dbo::ObjectNotFoundException e) {
                    std::cerr << e.what() << std::endl;
                    t2i.erase(object);
                } catch (dbo::StaleObjectException& e) {
                    std::cerr << e.what() << std::endl;
                    need_reread = true;
                    Wt::WDateTime new_time = cached_now +
                                             config::tracker::stale_object_delay;
                    t2i[object] = w2t.insert(std::make_pair(new_time, object));
                } catch (std::exception& e) { // database locked?
                    std::cerr << e.what() << std::endl;
                    Wt::WDateTime new_time = cached_now +
                                             config::tracker::unknown_error_delay;
                    t2i[object] = w2t.insert(std::make_pair(new_time, object));
                }
                w2t.erase(w2t_it);
            } else {
                break;
            }
        }
        BOOST_FOREACH (Object object, objects) {
            add_or_update_task_(object);
        }
        refresh_();
        mutex_.unlock();
    }
}

void TaskTracker::add_or_update_task_(const Object& object) {
    T2I_It t2i_it = t2i.find(object);
    if (t2i_it != t2i.end()) {
        // delete old
        W2T_It w2t_it = t2i_it->second;
        w2t.erase(w2t_it);
    }
    t2i[object] = w2t.insert(std::make_pair(now(), object));
}

void TaskTracker::refresh_() {
    if (!w2t.empty()) {
        Wt::WDateTime next_check = std::max(w2t.begin()->first, now() + config::tracker::delay);
        timer_.expires_at(next_check.toPosixTime()); // internally calls cancel
        timer_.async_wait(boost::bind(&TaskTracker::check_, this, _1));
    }
}

}

