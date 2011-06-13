//
#ifndef THECHESS_CONFIG_H_
#define THECHESS_CONFIG_H_

#include "time_intervals.hpp"
#include "model/UserClassification.hpp"
#include "model/CompetitionType.hpp"
#include "ThechessOptions.hpp"

namespace thechess {
namespace config {

const char* const wt_config_files[] = {
    "wt_config.xml",
    "~/.thechess.wt_config.xml",
    "/etc/thechess/wt_config.xml",
    "/etc/wt/wt_config.xml"
};
const int wt_config_files_size = 4;

const char* const username_regex = "[a-zA-Z][a-zA-Z0-9._-]{2,29}";
const char* const password_regex = "[a-zA-Z0-9._%+-@]{3,30}";

namespace min
{
    const Td limit_std = td_null;
    const Td limit_private_init = 5 * minute;
    const Td pause_limit_init = td_null;

    const int first_draw = 10 * 2;
}

namespace defaults
{
    const Td limit_std = 2 * minute;
    const Td limit_private_init = 2 * hour;
    const Td pause_limit_init = day;

    const int first_draw = 10 * 2;

    const double pause_factor = 0.5;

    const ThechessOptions::DatabaseType database_type = ThechessOptions::Sqlite3;
    const char* const database_value = "~/thechess.db";
    const int connections_in_pool = 10;
}

namespace max
{
    const int moves_init = 20 * 2;

    const Td limit_std = week;
    const Td limit_private_init = 4 * week;
    const Td pause_limit_init = week;

    const int first_draw = 5000 * 2;
}

namespace competition
{
    using namespace model;
    namespace min
    {
        const int max_rating = 900;
        const int min_rating = 100;
        const Classification max_classification = CLASSIFICATION_F;
        const Classification min_classification = NO_CLASSIFICATION;
        const Td min_recruiting_time = hour;
        const Td max_recruiting_time = 2 * hour;
        const int min_users = 5;
        const int max_users = 10;
        const int max_simultaneous_games = 1;
        const float games_factor = 0.01;
        const Td relax_time = day;
    }

    namespace defaults
    {
        const CompetitionType competition_type = STAGED;
        const int max_rating = 2000;
        const int min_rating = 500;
        const Classification max_classification = SUPER_GRANDMASTER;
        const Classification min_classification = NO_CLASSIFICATION;
        const Td min_recruiting_time = 3 * day;
        const Td max_recruiting_time = week;
        const int min_users = 8;
        const int max_users = 16;
        const int max_simultaneous_games = 8;
        const float games_factor = 1.0;
        const Td relax_time = day;
    }

    namespace max
    {
        const int max_rating = 3000;
        const int min_rating = 1700;
        const Classification max_classification = SUPER_GRANDMASTER;
        const Classification min_classification = NATIONAL_MASTER;
        const Td min_recruiting_time = 4 * week;
        const Td max_recruiting_time = 8 * week;
        const int min_users = 50;
        const int max_users = 100;
        const int max_simultaneous_games = 50;
        const float games_factor = 10.0;
        const Td relax_time = week;
    }
}

namespace elo
{
    const int init_rating = 1200;
    const int big_rating = 2400;
    const float big_K = 10;
    const int beginner_number_of_games = 30;
    const float beginner_K = 25;
    const float other_K = 15;
}

const int salt_length = 16;
const char crypt_id = '5'; // man 3 crypt

namespace tracker
{
    const Td delay = 0.01 * second;
    const Td dummy_timer_expiry_time = 100500 * day;
}

const int cookie_session_age = (5*365*day).total_seconds();

}
}

#endif // THECHESS_CONFIG_H_
