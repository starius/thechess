#ifndef THECHESS_MODEL_COMPETITIONTYPE_HPP_
#define THECHESS_MODEL_COMPETITIONTYPE_HPP_

namespace thechess {
namespace model {

enum CompetitionType
{
    CLASSICAL = 10,
    STAGED = 20,
    TEAM = 30
};
const CompetitionType competition_types[] = {CLASSICAL, STAGED, TEAM};
const int competition_types_size = 3;

}
}

#endif

