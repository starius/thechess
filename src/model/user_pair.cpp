/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>

#include "model/all.hpp"

namespace thechess {

UserPair::UserPair()
{ }

UserPair::UserPair(const UserPtr& first, const UserPtr& second) {
    BOOST_ASSERT(first != second);
    if (first < second) {
        first_ = first;
        second_ = second;
    } else {
        first_ = second;
        second_ = first;
    }
}

bool UserPair::operator<(const UserPair& o) const {
    return first_ < o.first() || (first_ == o.first() && second_ < o.second());
}

bool UserPair::operator==(const UserPair& other) const {
    return first_ == other.first() && second_ == other.second();
}

bool UserPair::operator!=(const UserPair& other) const {
    return !(*this == other);
}

}

