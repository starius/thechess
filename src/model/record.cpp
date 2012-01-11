/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/global.hpp"
#include "utils/time_intervals.hpp"

namespace thechess {

Record::Record(const UserPtr& init, Wt::WString name, Wt::WString descr):
    name_(name), description_(descr), init_(init),
    created_(now())
{ }

}

