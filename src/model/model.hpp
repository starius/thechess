/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_MODEL_HPP_
#define THECHESS_MODEL_MODEL_HPP_

#include <boost/lexical_cast.hpp>

#include <Wt/WString>
#include <Wt/WDateTime>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Dbo/collection>
#include <Wt/Dbo/ptr>
#include <Wt/WLogger> // FIXME otherwise Wt::log were not defined
#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/Auth/Dbo/UserDatabase>
#include <Wt/Wc/TimeDurationDbo.hpp>
#include <Wt/Wc/Gather.hpp>
#include <Wt/Wc/Planning.hpp>

#include "model/global.hpp"
#include "utils/time_intervals.hpp"
#include "model/elo_player.hpp"
#include "chess/Moves.hpp"

/** \file model/model.hpp Code, neede by model classes.
This file should be included by hpp files of models.

\ingroup model
*/

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

/** \name Id field for BD */
/* @{ */

namespace thechess {

/** Id field for BD */
struct BDId {
    /** User */
    UserPtr user;

    /** Type of data */
    Wt::Wc::Gather::DataType type;

    /** Value */
    std::string value;

    /** Default constructor */
    BDId();

    /** Constructor */
    BDId(const UserPtr& u, Wt::Wc::Gather::DataType t, const std::string& v);

    /** Comparison operator */
    bool operator==(const BDId& other) const;

    /** Comparison operator */
    bool operator<(const BDId& other) const;
};

/** Output stream operator for BDId */
std::ostream& operator<<(std::ostream& o, const BDId& id);

}

namespace Wt {
namespace Dbo {

/** Dbo field for BDId */
template <class Action>
void field(Action& action, thechess::BDId& id,
           const std::string& name, int /* size */ = -1) {
    field(action, id.user, name + "user");
    field(action, id.type, name + "type");
    field(action, id.value, name + "value", Wt::Wc::Gather::MAX_SIZE);
}

/** Dbo settings of BD model */
template<>
struct dbo_traits<thechess::BD> : public dbo_default_traits {
    /** Type if id */
    typedef thechess::BDId IdType;

    /** Invalid id */
    static IdType invalidId() {
        return IdType();
    }

    /** Disables surrogate id field */
    static const char* surrogateIdField() {
        return 0;
    }

    /** Disables version field */
    static const char* versionField() {
        return 0;
    }
};

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

#endif

