/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

//
#ifndef THECHESS_CONFIG_H_
#define THECHESS_CONFIG_H_

#include "utils/time_intervals.hpp"
#include "model/global.hpp"
#include "Options.hpp"

namespace thechess {

/** The namespace for configuration constants */
namespace config {

/** Files to search for Wt config */
const char* const WT_CONFIG_FILES[] = {
    "wt_config.xml",
    "~/.thechess.wt_config.xml",
    "/etc/thechess/wt_config.xml",
    "/etc/wt/wt_config.xml"
};

/** Size of WT_CONFIG_FILES_SIZE */
const int WT_CONFIG_FILES_SIZE = 4;

/** Regular expression of allowed user names */
const char* const USERNAME_REGEX = "[a-zA-Z][a-zA-Z0-9._-]{2,29}";

/** Regular expression of allowed passwords */
const char* const PASSWORD_REGEX = "[a-zA-Z0-9._%+-@]{3,30}";

/** The namespace for min allowed values of parameters */
namespace min {

/** Min renewable leeway */
const Td LIMIT_STD = TD_NULL;

/** Min non-renewable leeway */
const Td LIMIT_PRIVATE_INIT = 5 * MINUTE;

/** Min pause leeway */
const Td PAUSE_LIMIT_INIT = TD_NULL;

/** Min move number for draw */
const int FIRST_DRAW = 2;
}

/** The namespace for default values of parameters.
Values of constants from this namespace should be between values from
config::min and config::max.
*/
namespace defaults {

/** Default renewable leeway.
\sa GP::limit_std()
*/
const Td LIMIT_STD = 2 * MINUTE;

/** Default non-renewable leeway.
\sa GP::limit_private_init()
*/
const Td LIMIT_PRIVATE_INIT = 2 * HOUR;

/** Default pause leeway.
\sa GP::pause_limit_init()
*/
const Td PAUSE_LIMIT_INIT = DAY;

/** Default move number for draw.
\sa GP::first_draw()
*/
const int FIRST_DRAW = 10 * 2;

/** Default value of pause time input.
The actual value is a product of remaining pause leeway and this value.
\sa GameWidget
*/
const double PAUSE_FACTOR = 0.5;

/** Default Wt::Dbo backend.
\sa Options::database_type()
*/
const Options::DatabaseType DATABASE_TYPE = Options::SQLITE3;

/** Default value of Wt::Dbo backend configuration.
 - For Sqlite3, path to database
 - for Postgres, "user = user password = password dbname = dbname"

\sa Options::database_value()
*/
const char* const DATABASE_VALUE = "~/thechess.db";

/** The number of database connections in a pool.
\sa Options::connections_in_pool()
*/
const int CONNECTIONS_IN_POOL = 10;

/** Default value of norating.
\sa GP::norating()
*/
const bool NORATING = false;
}

/** The namespace for max allowed values of parameters */
namespace max {

/** Max number of predetermined half-moves.
\todo rename to HALFMOVES_INIT
*/
const int MOVES_INIT = 20 * 2;

/** Max renewable leeway */
const Td LIMIT_STD = WEEK;

/** Max non-renewable leeway */
const Td LIMIT_PRIVATE_INIT = 4 * WEEK;

/** Max pause leeway */
const Td PAUSE_LIMIT_INIT = WEEK;

/** Max move number for draw */
const int FIRST_DRAW = 5000 * 2;
}

/** The namespace for competition constants */
namespace competition {

/** The namespace for min allowed values of parameters */
namespace min {

/** Min value of max rating of member */
const int MAX_RATING = 900;

/** Min value of min rating of member */
const int MIN_RATING = 100;

/** Min value of max classification of member */
const Classification MAX_CLASSIFICATION = CLASSIFICATION_F;

/** Min value of min classification of member */
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

