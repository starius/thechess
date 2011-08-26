/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <cstddef>

namespace thechess {
namespace random {

unsigned int rand();

unsigned int rr(unsigned int stop);
unsigned int rr(unsigned int start, unsigned int stop);
unsigned int rr(unsigned int start, unsigned int stop, unsigned int step);

double drr(double start, double stop);

ptrdiff_t rand_for_shuffle(ptrdiff_t i);

}
}
