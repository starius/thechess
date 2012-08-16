/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_USER_PAIR_HPP_
#define THECHESS_MODEL_USER_PAIR_HPP_

#include "model/global.hpp"

namespace thechess {

/** Pair of user pointers */
class UserPair {
public:
    /** Create null user pair */
    UserPair();

    /** Constructor.
    \attention Asserts first != second
    \note stores min value to first(), max() -- to second()
    */
    UserPair(const UserPtr& first, const UserPtr& second);

    /** Comparison operator */
    bool operator<(const UserPair& other) const;

    /** Comparison operator */
    bool operator==(const UserPair& other) const;

    /** Comparison operator */
    bool operator!=(const UserPair& other) const;

    /** Boolean operator */
    operator bool() const {
        return first_ && second_;
    }

    /** Get min of two user pointers */
    const UserPtr& first() const {
        return first_;
    }

    /** Get max of two user pointers */
    const UserPtr& second() const {
        return second_;
    }

private:
    UserPtr first_;
    UserPtr second_;
};

}

#endif

