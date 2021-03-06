/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_THECHESS_SESSION_H_
#define THECHESS_THECHESS_SESSION_H_

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/WSignal>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/FixedSqlConnectionPool>
#include <Wt/Auth/Login>

#include "thechess-global.hpp"
#include "model/all.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

/** Descendant of dbo::Session
\ingroup server
\ingroup model
*/
class Session : public dbo::Session {
public:
    /** Constructor */
    Session(dbo::FixedSqlConnectionPool& pool);

    /** Constructor */
    Session(dbo::SqlConnection& connection);

    /** Create tables and initial objects, add objects to planning server */
    void reconsider(Server& server);

    /** Create an instance of sql connection class, using application server */
    static dbo::SqlConnection* new_connection(const Options& options);

    /** Access user database */
    UserDatabase& user_database() {
        return user_database_;
    }

    /** Current login state */
    Wt::Auth::Login& login() {
        return login_;
    }

    /** Return user */
    UserPtr user();

    /** Iterate through all games and recalculate user ratings.
    \warning Do not open a transaction before calling this!
    */
    void recalculate_game_rating();

    /** Iterate through all competitions and recalculate user ratings.
    \warning Do not open a transaction before calling this!
    */
    void recalculate_competition_rating();

    /** Iterate through all gp and set number of games referring it.
    \warning Do not open a transaction before calling this!
    */
    void recalculate_gp_games_size();

    /** Iterate through all cp and set number of competitions referring it.
    \warning Do not open a transaction before calling this!
    */
    void recalculate_cp_competitions_size();

    /** Iterate through all comments and set depth and index.
    \warning Do not open a transaction before calling this!
    */
    void recalculate_comments();

private:
    UserDatabase user_database_;
    Wt::Auth::Login login_;

    void map_classes();

    UserPtr add_user(const Server& server, const Wt::WString& name,
                     const Wt::WString& password);

    UserPtr auth_info_to_user(const AuthInfoPtr& auth_info);
};

}

#endif

