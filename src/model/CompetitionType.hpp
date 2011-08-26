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
const CompetitionType competition_types[] = {CLASSICAL, STAGED, TEAM};
const int competition_types_size = 3;

}

#endif

