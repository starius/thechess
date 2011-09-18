/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_GLOBAL_HPP_
#define THECHESS_MODEL_GLOBAL_HPP_

#include <string>
#include <boost/lexical_cast.hpp>

#include <Wt/WString>
#include <Wt/WDateTime>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Dbo/collection>
#include <Wt/Dbo/ptr>

#include "utils/time_intervals.hpp"
#include "model/EloPlayer.hpp"
#include "model/Object.hpp"

namespace dbo = Wt::Dbo;

/** \file model/global.hpp Declarations of all models
This file should be included by hpp files of models
\ingroup model
*/

/** \name Declarations of all models */
/* @{ */

namespace thechess {

class User;
class CookieSession;
class Game;
class Competition;

#ifndef DOXYGEN_ONLY
typedef dbo::ptr<User> UserPtr;
typedef dbo::ptr<CookieSession> CookieSessionPtr;
typedef dbo::ptr<Game> GamePtr;
typedef dbo::ptr<Competition> CompetitionPtr;
#else
typedef User* UserPtr;
typedef CookieSession* CookieSessionPtr;
typedef Game* GamePtr;
typedef Competition* CompetitionPtr;
#endif

#ifndef DOXYGEN_ONLY
typedef dbo::collection<UserPtr> Users;
typedef dbo::collection<CookieSessionPtr> CookieSessions;
typedef dbo::collection<GamePtr> Games;
typedef dbo::collection<CompetitionPtr> Competitions;
#else
typedef User* Users;
typedef CookieSession* CookieSessions;
typedef Game* Games;
typedef Competition* Competitions;
#endif

typedef std::vector<UserPtr> UsersVector;
typedef std::vector<CookieSessionPtr> CookieSessionsVector;
typedef std::vector<GamePtr> GamesVector;
typedef std::vector<CompetitionPtr> CompetitionsVector;

}

/* @} */

/** \name Field for Td */
/* @{ */

namespace Wt {
namespace Dbo {

class SqlConnection;
class SqlStatement;

template<>
struct sql_value_traits<thechess::Td, void> {
    static const bool specialized = true;

    static const char* type(SqlConnection* conn, int size);
    static void bind(const thechess::Td& v, SqlStatement* statement,
                     int column, int size);
    static bool read(thechess::Td& v, SqlStatement* statement,
                     int column, int size);
};

}
}

/* @} */

/** \name Field for EloPlayer */
/* @{ */

namespace Wt {
namespace Dbo {

template<class Action>
void field(Action& a, thechess::EloPlayer& p, const std::string& name) {
    field(a, const_cast<int&>(p.elo()), name + "_elo");
    field(a, const_cast<int&>(p.all()), name + "_all");
    field(a, const_cast<int&>(p.wins()), name + "_wins");
    field(a, const_cast<int&>(p.fails()), name + "_fails");
}

}
}

/* @} */

/** \name Id field for CookieSession */
/* @{ */

namespace Wt {
namespace Dbo {

template<>
struct dbo_traits<thechess::CookieSession> :
    public dbo_default_traits {
    typedef std::string IdType;

    static IdType invalidId() {
        return std::string();
    }

    static const char* surrogateIdField() {
        return 0;
    }
};

}
}

/* @} */

#endif

