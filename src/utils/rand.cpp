/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <climits>

#include <Wt/WRandom>

namespace thechess {

const unsigned int UINT_MIN = 0;

unsigned int rand() {
    return Wt::WRandom::get();
}

unsigned int rr(unsigned int stop) {
    return rand() % stop;
}

unsigned int rr(unsigned int start, unsigned int stop) {
    return start + rand() % (stop - start);
}

unsigned int rr(unsigned int start, unsigned int stop, unsigned int step) {
    return start + (rand() % ((stop - start) / step)) * step;
}

double drr(double start, double stop) {
    double stop_start = stop - start;
    const double max_min = UINT_MAX - UINT_MIN;
    return start + double(rand()) / max_min * stop_start;
}

ptrdiff_t rand_for_shuffle(ptrdiff_t i) {
    return rr(static_cast<unsigned int>(i));
}

}

