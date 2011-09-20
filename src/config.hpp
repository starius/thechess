/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

//
#ifndef THECHESS_CONFIG_H_
#define THECHESS_CONFIG_H_

#include "utils/time_intervals.hpp"
#include "model/global.hpp"
#include "Options.hpp"

namespace thechess {
namespace config {

const char* const WT_CONFIG_FILES[] = {
    "wt_config.xml",
    "~/.thechess.wt_config.xml",
    "/etc/thechess/wt_config.xml",
    "/etc/wt/wt_config.xml"
};
const int WT_CONFIG_FILES_SIZE = 4;

const char* const USERNAME_REGEX = "[a-zA-Z][a-zA-Z0-9._-]{2,29}";
const char* const PASSWORD_REGEX = "[a-zA-Z0-9._%+-@]{3,30}";

namespace min {
const Td LIMIT_STD = TD_NULL;
const Td LIMIT_PRIVATE_INIT = 5 * MINUTE;
const Td PAUSE_LIMIT_INIT = TD_NULL;

const int FIRST_DRAW = 2;
}

namespace defaults {
const Td LIMIT_STD = 2 * MINUTE;
const Td LIMIT_PRIVATE_INIT = 2 * HOUR;
const Td PAUSE_LIMIT_INIT = DAY;

const int FIRST_DRAW = 10 * 2;

const double PAUSE_FACTOR = 0.5;

const Options::DatabaseType DATABASE_TYPE = Options::SQLITE3;
const char* const DATABASE_VALUE = "~/thechess.db";
const int CONNECTIONS_IN_POOL = 10;

const bool NORATING = false;
}

namespace max {
const int MOVES_INIT = 20 * 2;

const Td LIMIT_STD = WEEK;
const Td LIMIT_PRIVATE_INIT = 4 * WEEK;
const Td PAUSE_LIMIT_INIT = WEEK;

const int FIRST_DRAW = 5000 * 2;
}

namespace competition {
namespace min {
const int MAX_RATING = 900;
const int MIN_RATING = 100;
const Classification MAX_CLASSIFICATION = CLASSIFICATION_F;
const Classification MIN_CLASSIFICATION = NO_CLASSIFICATION;
const Td FORCE_START_DELAY = MINUTE;
const Td MIN_RECRUITING_TIME = MINUTE;
const Td MAX_RECRUITING_TIME = 2 * HOUR;
const int MIN_USERS = 2;
const int MAX_USERS = 5;
const int MAX_SIMULTANEOUS_GAMES = 1;
const float GAMES_FACTOR = 0.01;
const Td RELAX_TIME = MINUTE;
const int MIN_SUBSTAGES = 1;
const int INCREMENT_SUBSTAGES = 0;
}

namespace defaults {
const CompetitionType COMPETITION_TYPE = STAGED;
const int MAX_RATING = 2000;
const int MIN_RATING = 500;
const Classification MAX_CLASSIFICATION = SUPER_GRANDMASTER;
const Classification MIN_CLASSIFICATION = NO_CLASSIFICATION;
const Td FORCE_START_DELAY = DAY;
const Td MIN_RECRUITING_TIME = 3 * DAY;
const Td MAX_RECRUITING_TIME = WEEK;
const int MIN_USERS = 8;
const int MAX_USERS = 16;
const int MAX_SIMULTANEOUS_GAMES = 8;
const float GAMES_FACTOR = 1.0;
const Td RELAX_TIME = DAY;
const int MIN_SUBSTAGES = 3;
const int INCREMENT_SUBSTAGES = 1;

const bool NORATING = true;
}

namespace max {
const int MAX_RATING = 3000;
const int MIN_RATING = 1700;
const Classification MAX_CLASSIFICATION = SUPER_GRANDMASTER;
const Classification MIN_CLASSIFICATION = NATIONAL_MASTER;
const Td FORCE_START_DELAY = WEEK;
const Td MIN_RECRUITING_TIME = 4 * WEEK;
const Td MAX_RECRUITING_TIME = 8 * WEEK;
const int MIN_USERS = 50;
const int MAX_USERS = 100;
const int MAX_SIMULTANEOUS_GAMES = 50;
const float GAMES_FACTOR = 10.0;
const Td RELAX_TIME = WEEK;
const int MIN_SUBSTAGES = 15;
const int INCREMENT_SUBSTAGES = 3;
}

const unsigned int MAX_NAME = 50;
const unsigned int MAX_DESCRIPTION = 1000;
}

namespace elo {
const int INIT = 1200;
const int BIG = 2400;
const float BIG_K = 10;
const int BEGINNER_ALL = 30;
const float BEGINNER_K = 25;
const float OTHER_K = 15;
}

const int SALT_LENGTH = 16;
const char CRYPT_ID = '5'; // man 3 crypt

namespace tracker {
const Td DELAY = 0.01 * SECOND;
const Td DUMMY_TIMER_EXPIRY_TIME = 100500 * DAY;
const Td STALE_OBJECT_DELAY = SECOND;
const Td UNKNOWN_ERROR_DELAY = 2 * SECOND;
}

const int COOKIE_SESSION_AGE = (5 * 365 * DAY).total_seconds();

}
}

#endif

