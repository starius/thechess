#ifndef THECHESS_TASKTRACKER_HPP_
#define THECHESS_TASKTRACKER_HPP_

#include <Wt/Dbo/Session>
namespace dbo = Wt::Dbo;

namespace thechess {
namespace tracker {

enum TaskType
{
    Game,
    Tournament
};

void add_or_update_task(TaskType type, int id, dbo::Session* session); // under Transaction
void check(dbo::Session* session); // under Transaction

}
}

#endif
