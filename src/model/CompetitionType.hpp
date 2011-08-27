/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMPETITIONTYPE_HPP_
#define THECHESS_MODEL_COMPETITIONTYPE_HPP_

namespace thechess {

enum CompetitionType {
    CLASSICAL = 10,
    STAGED = 20,
    TEAM = 30
};
const CompetitionType COMPETITION_TYPES[] = {CLASSICAL, STAGED, TEAM};
const int COMPETITION_TYPES_SIZE = 3;

}

#endif

