/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_AWARDS_HPP_
#define THECHESS_AWARDS_HPP_

#include <Wt/WGlobal>

#include "model/all.hpp"

namespace thechess {

/** Number of awards */
struct Awards {
    /** Number of competition-related awards.
    floor(log_2(wins)) + 1.
    */
    int competitions;

    /** Number of game-related awards of type 1.
    0 if other game-related awards are given.
    1 if games >= 100 or wins >= 50.
    0 otherwise.
    */
    int game_1;

    /** Number of game-related awards of type 2.
    0 if other game-related awards are given.
    1 if games >= 250 or wins >= 125.
    0 otherwise.
    */
    int game_2;

    /** Number of game-related awards of type 3.
    max(games // 500, wins // 250).
    */
    int game_3;
};

/** Return number of awards */
Awards awards_of(const UserPtr& user);

/** Return widget with awards */
Wt::WWidget* awards_widget(const UserPtr& user);

}

#endif

