#include <cstdio>
#include <map>
#include <list>
#include <utility>

#include <Wt/WDateTime>

#include "TaskTracker.hpp"
#include "time_intervals.hpp"
#include "ThechessApplication.hpp"
#include "ThechessSession.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"

namespace thechess {
namespace tracker {

using model::GamePtr;
using model::UserPtr;

struct Task
{
    Task(TaskType t, int i): type(t), id(i) {}

    TaskType type;
    int id;
};

bool operator < (const Task& a, const Task& b)
{
    return a.id < b.id;
}

typedef std::multimap<Wt::WDateTime, Task> W2T;
typedef W2T::iterator W2T_It;
typedef std::map<Task, W2T_It> T2I;
typedef T2I::iterator T2I_It;

W2T w2t;
T2I t2i;

Wt::WDateTime process_task(const Task& task, dbo::Session* session)
{
    Wt::WDateTime result;
    if (task.type == Game)
    {
        GamePtr game = session->load<model::Game>(task.id);
        // FIXME game.reread();
        game.modify()->check_impl_();
        result = game->next_check();
    }
    return result;
}

void add_or_update_task(TaskType type, int id, dbo::Session* session)
{
    printf("%d\n", id);
    Task task(type, id);
    check(session);
    Wt::WDateTime new_time = process_task(task, session);
    T2I_It t2i_it = t2i.find(task);
    if (t2i_it != t2i.end())
    {
        // delete old
        W2T_It w2t_it = t2i_it->second;
        w2t.erase(w2t_it);
    }
    t2i[task] = w2t.insert(std::make_pair(new_time, task));
}

void check(dbo::Session* session)
{
    Wt::WDateTime cached_now = now();
    while (!w2t.empty())
    {
        W2T_It w2t_it = w2t.begin();
        const Wt::WDateTime& time = w2t_it->first;
        if (cached_now >= time)
        {
            const Task& task = w2t_it->second;
            Wt::WDateTime new_time = process_task(task, session);
            if (new_time.isValid() && new_time > cached_now)
            {
                // update
                t2i[task] = w2t.insert(std::make_pair(new_time, task));
            }
            else
            {
                // delete
                t2i.erase(task);
            }
            w2t.erase(w2t_it);
        }
        else
        {
            break;
        }
    }
}

}
}
