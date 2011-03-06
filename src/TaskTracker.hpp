#ifndef THECHESS_TASKTRACKER_HPP_
#define THECHESS_TASKTRACKER_HPP_

namespace thechess {
namespace tracker {

enum TaskType
{
    Game,
    Tournament
};

void add_or_update_task(TaskType type, int id); // under Transaction
void check(); // under Transaction

}
}

#endif
