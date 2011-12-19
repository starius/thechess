/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
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
#include "model/elo_player.hpp"
#include "model/object.hpp"
#include "chess/Moves.hpp"

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
class GP;
class Game;
class CP;
class Competition;

#ifndef DOXYGEN_ONLY
typedef dbo::ptr<User> UserPtr;
typedef dbo::ptr<CookieSession> CookieSessionPtr;
typedef dbo::ptr<GP> GPPtr;
typedef dbo::ptr<Game> GamePtr;
typedef dbo::ptr<CP> CPPtr;
typedef dbo::ptr<Competition> CompetitionPtr;
#else
typedef User* UserPtr;
typedef CookieSession* CookieSessionPtr;
typedef GP* GPPtr;
typedef Game* GamePtr;
typedef CP* CPPtr;
typedef Competition* CompetitionPtr;
#endif

#ifndef DOXYGEN_ONLY
typedef dbo::collection<UserPtr> Users;
typedef dbo::collection<CookieSessionPtr> CookieSessions;
typedef dbo::collection<GPPtr> GPs;
typedef dbo::collection<GamePtr> Games;
typedef dbo::collection<CPPtr> CPs;
typedef dbo::collection<CompetitionPtr> Competitions;
#else
typedef User* Users;
typedef CookieSession* CookieSessions;
typedef GP* GPs;
typedef Game* Games;
typedef CP* CPs;
typedef Competition* Competitions;
#endif

typedef std::vector<UserPtr> UsersVector;
typedef std::vector<CookieSessionPtr> CookieSessionsVector;
typedef std::vector<GPPtr> GPsVector;
typedef std::vector<GamePtr> GamesVector;
typedef std::vector<CPPtr> CPsVector;
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

/** \name Field for Moves */
/* @{ */

namespace Wt {
namespace Dbo {

template<class Action>
void field(Action& a, thechess::Moves& moves, const std::string& name) {
    field(a, moves.as_svuc(), name + "_moves");
}

}
}

/* @} */

/** \name Field for WhiteBlack */
/* @{ */

namespace thechess {

/** Template struct for paired values for Game model.
This struct stores a pair of values: for white and for black.
*/
template<typename T>
struct WhiteBlack {
    /** Default constructor */
    WhiteBlack()
    { }

    /** Initialize both values with one argument */
    WhiteBlack(const T& common):
        white(common), black(common)
    { }

    /** Initialize values with two arguments */
    WhiteBlack(const T& w, const T& b):
        white(w), black(b)
    { }

    /** Get value corresponding to the color */
    const T& operator[](Piece::Color color) const {
        return color == Piece::WHITE ? white : black;
    }

    /** Get value corresponding to the color */
    T& operator[](Piece::Color color) {
        return color == Piece::WHITE ? white : black;
    }

    /** Value corresponding to white color */
    T white;

    /** Value corresponding to black color */
    T black;
};

}

namespace Wt {
namespace Dbo {

template<class Action, typename T>
void field(Action& a, thechess::WhiteBlack<T>& wb, const std::string& name) {
    field(a, wb.white, name + "_white");
    field(a, wb.black, name + "_black");
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

/** \name Type of Competition and CP */
/* @{ */

namespace thechess {

/** Competition type */
enum CompetitionType {
    CLASSICAL = 10, /**< Classical competition */
    STAGED = 20, /**< Staged competition */
    TEAM = 30 /**< Team competition */
};

/** Array of all CompetitionType members */
const CompetitionType COMPETITION_TYPES[] = {CLASSICAL, STAGED, TEAM};

/** Length of COMPETITION_TYPES */
const int COMPETITION_TYPES_SIZE = 3;

}

/* @} */

/** \name Classification of User */
/* @{ */

namespace thechess {

/** Classification of player */
enum Classification {
    SUPER_GRANDMASTER = 110, /**< Super grandmaster */
    GRANDMASTER = 100, /**< Grandmaster */
    INTERNATIONAL_MASTER = 90, /**< International master */
    NATIONAL_MASTER = 80, /**< National master */
    EXPERT = 70, /**< Expert (master) */
    CLASSIFICATION_A = 50, /**< A (I) */
    CLASSIFICATION_B = 45, /**< B (II) */
    CLASSIFICATION_C = 40, /**< C (III) */
    CLASSIFICATION_D = 35, /**< D (IV) */
    CLASSIFICATION_E = 30, /**< E (V, newbie) */
    CLASSIFICATION_F = 25, /**< F (newbie) */
    CLASSIFICATION_G = 20, /**< G (newbie) */
    CLASSIFICATION_H = 15, /**< H (newbie) */
    CLASSIFICATION_I = 10, /**< I (newbie) */
    CLASSIFICATION_J = 5, /**< J (newbie) */
    NO_CLASSIFICATION = 0 /**< Player without assigned classification */
};

/** Array of all Classification members */
const Classification CLASSIFICATIONS[] = {
    SUPER_GRANDMASTER, GRANDMASTER, INTERNATIONAL_MASTER,
    NATIONAL_MASTER, EXPERT, CLASSIFICATION_A, CLASSIFICATION_B,
    CLASSIFICATION_C, CLASSIFICATION_D, CLASSIFICATION_E, CLASSIFICATION_F,
    CLASSIFICATION_G, CLASSIFICATION_H, CLASSIFICATION_I, CLASSIFICATION_J,
    NO_CLASSIFICATION
};

/** Length of CLASSIFICATIONS */
const int CLASSIFICATIONS_SIZE = 16;

}

/* @} */

#endif

