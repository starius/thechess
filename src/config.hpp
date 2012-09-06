/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_CONFIG_H_
#define THECHESS_CONFIG_H_

#include "utils/time_intervals.hpp"
#include "model/global.hpp"
#include "thechess-global.hpp"

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
const DatabaseType DATABASE_TYPE = SQLITE3;

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

/** Rights of regular user */
const UserRights REGULAR_USER_RIGHTS = REGULAR_USER;

/** Rights of banned user */
const UserRights BANNED_IP_USER_RIGHTS = NONE;

/** Rights of Anonymous */
const UserRights ANONYMOUS_RIGHTS = REGULAR_USER;

/** Comment id for main page */
const int MAIN_PAGE_CONTENT_ID = -1;

/** Comment id for footer */
const int FOOTER_CONTENT_ID = -1;

/** Comment id for top */
const int TOP_LOGGED_IN_CONTENT_ID = -1;

/** Time of inactivity after which user is considered offline */
const Td AWAY_TIMEOUT = 5 * MINUTE;

/** Default settings of user */
const UserSettings DEFAULT_SETTINGS = SWITCH_DEFAULT;

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

/** The namespace for competition constants.
\see CP
*/
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

/** Time after which confirmed games would be started automatically */
const Td FORCE_START_DELAY = MINUTE;

/** Min recruiting time competition to start */
const Td MIN_RECRUITING_TIME = MINUTE;

/** Time after which competition would be canceled in case not started */
const Td MAX_RECRUITING_TIME = 2 * HOUR;

/** Min number of members competition to start */
const int MIN_USERS = 2;

/** Max allowed number of members */
const int MAX_USERS = 5;

/** Max number of simultaneous games by user */
const int MAX_SIMULTANEOUS_GAMES = 1;

/** Factor of games number */
const float GAMES_FACTOR = 0.01;

/** Time after which created games would be confirmed automatically */
const Td RELAX_TIME = MINUTE;

/** Min number of substages played in case of draw before no-draw game */
const int MIN_SUBSTAGES = 1;

/** Increment of the number of substages per each substage */
const int INCREMENT_SUBSTAGES = 0;

}

/** The namespace for default values of parameters */
namespace defaults {

/** Competition type, selected by default */
const CompetitionType COMPETITION_TYPE = STAGED;

/** Min value of max rating of member */
const int MAX_RATING = 2000;

/** Min value of min rating of member */
const int MIN_RATING = 500;

/** Min value of max classification of member */
const Classification MAX_CLASSIFICATION = SUPER_GRANDMASTER;

/** Min value of min classification of member */
const Classification MIN_CLASSIFICATION = NO_CLASSIFICATION;

/** Time after which confirmed games would be started automatically */
const Td FORCE_START_DELAY = DAY;

/** Min recruiting time competition to start */
const Td MIN_RECRUITING_TIME = 3 * DAY;

/** Time after which competition would be canceled in case not started */
const Td MAX_RECRUITING_TIME = WEEK;

/** Min number of members competition to start */
const int MIN_USERS = 8;

/** Max allowed number of members */
const int MAX_USERS = 16;

/** Max number of simultaneous games by user */
const int MAX_SIMULTANEOUS_GAMES = 8;

/** Factor of games number */
const float GAMES_FACTOR = 1.0;

/** Time after which created games would be confirmed automatically */
const Td RELAX_TIME = DAY;

/** Min number of substages played in case of draw before no-draw game */
const int MIN_SUBSTAGES = 3;

/** Increment of the number of substages per each substage */
const int INCREMENT_SUBSTAGES = 1;

/** Default value of norating.
\sa GP::norating()
*/
const bool NORATING = true;

}

/** The namespace for max allowed values of parameters */
namespace max {

/** Min value of max rating of member */
const int MAX_RATING = 3000;

/** Min value of min rating of member */
const int MIN_RATING = 1700;

/** Min value of max classification of member */
const Classification MAX_CLASSIFICATION = SUPER_GRANDMASTER;

/** Min value of min classification of member */
const Classification MIN_CLASSIFICATION = NATIONAL_MASTER;

/** Time after which confirmed games would be started automatically */
const Td FORCE_START_DELAY = WEEK;

/** Min recruiting time competition to start */
const Td MIN_RECRUITING_TIME = 4 * WEEK;

/** Time after which competition would be canceled in case not started */
const Td MAX_RECRUITING_TIME = 8 * WEEK;

/** Min number of members competition to start */
const int MIN_USERS = 50;

/** Max allowed number of members */
const int MAX_USERS = 100;

/** Max number of simultaneous games by user */
const int MAX_SIMULTANEOUS_GAMES = 50;

/** Factor of games number */
const float GAMES_FACTOR = 10.0;

/** Time after which created games would be confirmed automatically */
const Td RELAX_TIME = WEEK;

/** Min number of substages played in case of draw before no-draw game */
const int MIN_SUBSTAGES = 15;

/** Increment of the number of substages per each substage */
const int INCREMENT_SUBSTAGES = 3;

}

}

/** The namespace for default values of Elo rating system parameters.
\see EloPlayer
*/
namespace elo {

/** Initial rating */
const int INIT = 1200;

/** If user rating is greater or equal to this value, K = BIG_K */
const int BIG = 2400;

/** If user rating is greater or equal to BIG, K = this value */
const float BIG_K = 10;

/** If user games number is greater or equal to this value, K = BEGINNER_K */
const int BEGINNER_ALL = 30;

/** If user games number is greater or equal to BEGINNER_ALL, K = this value */
const float BEGINNER_K = 25;

/** The value of K used otherwise */
const float OTHER_K = 15;

}

/** The namespace for values of planning server parameters.
\see Planning
*/
namespace tracker {

/** Standard delay for all tasks.
\see PlanningServer::set_delay()
*/
const Td DELAY = 0.01 * SECOND;

/** Delay for task re-processing in case of stale object exception.
The valus is selected each time from this range (min, max).
\see Object::process()
*/
const std::pair<Td, Td> STALE_OBJECT_DELAY = std::make_pair(SECOND, 5 * SECOND);

/** Delay for task re-processing in case of unknown std::exception.
The valus is selected each time from this range (min, max).
\see Object::process()
*/
const std::pair<Td, Td> UNKNOWN_ERROR_DELAY = std::make_pair(SECOND, MINUTE);

}

/** Max length of record name */
const unsigned int MAX_NAME = 50;

/** Max length of record description */
const unsigned int MAX_DESCRIPTION = 1000;

/** Duration for an cookie-based authenticaton to remain valid */
const Td COOKIE_AGE = 5 * 366 * DAY;

/** Authentication token cookie name */
const char* const COOKIE_NAME = "thechessauth";

/** Thether user information should be collected.
see wt-classes/Gather.
*/
const bool GATHERING = true;

}

}

#endif

