//
#ifndef THECHESS_CONFIG_H_
#define THECHESS_CONFIG_H_

#include "time_intervals.hpp"
#include "model/UserClassification.hpp"
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

    namespace competition
    {
        const int max_rating = 3000;
        const int min_rating = 100;
        using namespace model;
        const Classification max_classification = SUPER_GRANDMASTER;
        const Classification min_classification = NO_CLASSIFICATION;
        const Td max_recruiting_time = 3 * day;
        const int max_users = 16;
        const int max_simultaneous_games = 8;
        const float games_factor = 1.0;
        const Td relax_time = day;
    }

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

}
}

#endif // THECHESS_CONFIG_H_
