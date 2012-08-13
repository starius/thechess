/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_KICK_HPP_
#define THECHESS_KICK_HPP_

#include "notify.hpp"

namespace thechess {

/** Object, kicking removed users.

\ingroup server
*/
class Kick : public Notifiable {
public:
    /** Constructor.
    Must be called if tApp->user() returns valid user.
    */
    Kick();

    /** User notification listner */
    void notify(EventPtr);
};

}

#endif

