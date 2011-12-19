/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cstddef>

namespace thechess {

unsigned int rand();

unsigned int rr(unsigned int stop);
unsigned int rr(unsigned int start, unsigned int stop);
unsigned int rr(unsigned int start, unsigned int stop, unsigned int step);

double drr(double start, double stop);

ptrdiff_t rand_for_shuffle(ptrdiff_t i);

}

