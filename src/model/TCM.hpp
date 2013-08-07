/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_TCM_HPP_
#define THECHESS_MODEL_TCM_HPP_

#include "model/model.hpp"

namespace thechess {

/** Team competition member.

\ingroup model
*/
class TCM {
public:
    /** Constructor for Wt::Dbo internal usage */
    TCM();

    /** Create instance to be added to database */
    TCM(const TeamPtr& team, const CompetitionPtr& competition,
        const UserPtr& u);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id_, "");
    }

    /** Team */
    const TeamPtr& team() const {
        return id_.team;
    }

    /** Competition */
    const CompetitionPtr& competition() const {
        return id_.competition;
    }

    /** User */
    const UserPtr& user() const {
        return id_.user;
    }

private:
    TCMId id_;
};

}

DBO_EXTERN_TEMPLATES(thechess::TCM);

#endif

