/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_TIME_INTERVALS_
#define THECHESS_TIME_INTERVALS_

#include <Wt/WDateTime>
#include <Wt/Wc/TimeDuration.hpp>

namespace thechess {

using namespace Wt::Wc::td;

/** Time duration */
typedef TimeDuration Td;

inline Wt::WDateTime now() {
    return Wt::WDateTime::currentDateTime();
}

inline std::string td2str(const Td& duration) {
    return duration;
}

}

#endif

