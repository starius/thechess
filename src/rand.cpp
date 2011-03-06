
#include <boost/random/mersenne_twister.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace thechess {
namespace random {

class WCaptchaRandom : public boost::mt19937
{
public:
    WCaptchaRandom() : boost::mt19937()
    {
        long s = boost::posix_time::microsec_clock::universal_time()
            .time_of_day().total_microseconds();
        seed(s);
    }

} r_;

int rand()
{
    return r_();
}

int rr(int stop)
{
    return r_() % stop;
}

int rr(int start, int stop)
{
    return start + r_() % (stop - start);
}

int rr(int start, int stop, int step)
{
    return start + (r_() % ((stop - start) / step)) * step;
}

double drr(double start, double stop)
{
    double stop_start = stop - start;
    double max_min = r_.max() - r_.min();
    return start + double(r_()) / max_min * stop_start;
}

}
}
