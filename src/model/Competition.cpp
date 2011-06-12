
#include "model/Competition.hpp"
#include "config.hpp"

namespace thechess {
namespace model {

void Competition::initialize_()
{
    CompetitionParameters::initialize_();
    state_ = RECRUITING;
    created_ = now();
}

Competition* Competition::create_new()
{
    Competition* c = new Competition();
    c->initialize_();
    return c;
}


}
}

