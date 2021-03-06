/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_OPTIONS_HPP_
#define THECHESS_OPTIONS_HPP_

#include <string>

#include <Wt/WGlobal>

#include "thechess-global.hpp"
#include "model/global.hpp"
#include "utils/time_intervals.hpp"

namespace thechess {

/** Settings of thechess program.
The configuration is read from wt_config.xml (Runtime Properties section).

\ingroup server
*/
class Options {
public:
    /** Constructor */
    Options(const Wt::WServer& server);

    /** Get Wt::Dbo backend type */
    DatabaseType database_type() const {
        return database_type_;
    }

    /** Get Wt::Dbo backend configuration */
    const std::string& database_value() const {
        return database_value_;
    }

    /** Get The number of database connections in a pool */
    int connections_in_pool() const {
        return connections_in_pool_;
    }

    /** Rights of new user (not from banned IP) */
    UserRights regular_user_rights() const {
        return regular_user_rights_;
    }

    /** Rights of new user (from banned IP) */
    UserRights banned_ip_user_rights() const {
        return banned_ip_user_rights_;
    }

    /** Rights of anonymous.
    Anonymous doesn't have all possible rights, even if mentioned here.
    Anonymous with banned IP has bitwise AND of
    anonymous_rights() and banned_ip_user_rights().
    */
    UserRights anonymous_rights() const {
        return anonymous_rights_;
    }

    /** Mask of rights of vacation user */
    UserRights vacation_rights() const {
        return vacation_rights_;
    }

    /** Id of comment, which text is used as main page contents */
    int main_page_content_id() const;

    /** Id of comment, which text is used as page footer */
    int footer_content_id() const;

    /** Id of comment, which text is shown right to AuthWidget when loggen in */
    int top_logged_in_content_id() const {
        return top_logged_in_content_id_;
    }

    /** Set Id of comment, which text is shown right to AuthWidget */
    void set_top_logged_in_content_id(int id) {
        top_logged_in_content_id_ = id;
    }

    /** Time of inactivity after which user is considered offline */
    const Td& away_timeout() const {
        return away_timeout_;
    }

    /** Default user settings */
    UserSettings default_settings() const {
        return default_settings_;
    }

    /** String used as value of PGN [Site] tag */
    const std::string& pgn_site() const {
        return pgn_site_;
    }

    /** ID of champion shown on MainPage */
    int champion_id() const {
        return champion_id_;
    }

    /** Number of best players shown as main page */
    int best_players_shown() const {
        return best_players_shown_;
    }

    /** ID of comment with user agreement (or -1) */
    int user_agreement_id() const {
        return user_agreement_id_;
    }

    /** Max time non-competition game can be proposed or confirmed.
    After that time game will be cancelled.
    When game is confirmed, thislimit is "renewed".
    */
    const Td& game_max_preactive() const {
        return game_max_preactive_;
    }

    /** Get min value of first_draw.
    It affects all GP, even created before.
    */
    int min_first_draw() const {
        return min_first_draw_;
    }

    /** Get max sessions number allowed from one IP */
    int max_sessions() const {
        return max_sessions_;
    }

    /** Get max sessions number allowed from one IP from whitelist */
    int whitelist_max_sessions() const {
        return whitelist_max_sessions_;
    }

    /** Time difference from UTC */
    const Td& time_diff() {
        return time_diff_;
    }

    /** Widgets cache size */
    int widgets_cache_size() const {
        return widgets_cache_size_;
    }

    /** return if the IP is in whitelist */
    bool ip_in_whitelist(const std::string& ip) const;

    /** Return options used in current application */
    static Options* instance();

private:
    const Wt::WServer* server_; // should not be used after construction!

    typedef std::map<std::string, int> Locale2Int;

    DatabaseType database_type_;
    std::string database_value_;
    int connections_in_pool_;
    UserRights regular_user_rights_;
    UserRights banned_ip_user_rights_;
    UserRights anonymous_rights_;
    UserRights vacation_rights_;
    int main_page_content_id_;
    Locale2Int main_page_content_;
    int footer_content_id_;
    Locale2Int footer_content_;
    int top_logged_in_content_id_;
    Td away_timeout_;
    UserSettings default_settings_;
    std::string pgn_site_;
    int champion_id_;
    int best_players_shown_;
    int user_agreement_id_;
    Td game_max_preactive_;
    int min_first_draw_;
    int max_sessions_;
    std::vector<std::string> whitelist_; // sorted
    int whitelist_max_sessions_;
    Td time_diff_;
    int widgets_cache_size_;

    bool read_int_value(const std::string& name, int& value);
    bool read_seconds(const std::string& name, Td& value);
    bool read_locales(const std::string& name, int& main, Locale2Int& locales);

    int locales_id(const Locale2Int& locales, int d) const;
};

}

#endif

