
#include <climits>

#include <Wt/WRandom>

namespace thechess {
namespace random {

const unsigned int uint_min = 0;
const unsigned int uint_max = UINT_MAX;

unsigned int rand()
{
    return Wt::WRandom::get();
}

unsigned int rr(unsigned int stop)
{
    return rand() % stop;
}

unsigned int rr(unsigned int start, unsigned int stop)
{
    return start + rand() % (stop - start);
}

unsigned int rr(unsigned int start, unsigned int stop, unsigned int step)
{
    return start + (rand() % ((stop - start) / step)) * step;
}

double drr(double start, double stop)
{
    double stop_start = stop - start;
    const double max_min = uint_max - uint_min;
    return start + double(rand()) / max_min * stop_start;
}

ptrdiff_t rand_for_shuffle(ptrdiff_t i)
{
    return rr(static_cast<unsigned int>(i));
}

}
}
