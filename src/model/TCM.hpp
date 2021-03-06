/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_TCM_HPP_
#define THECHESS_MODEL_TCM_HPP_

#include <map>

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
        const UserPtr& user);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::id(a, id_, "");
        dbo::belongsTo(a, team_, "team");
    }

    /** Team */
    const TeamPtr& team() const {
        return team_;
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
    TeamPtr team_;
};

/** Add relations user to his team for the competition to the map */
void tcm_map_user_to_team(User2Team& result, const CompetitionPtr& competition);

/** Map team to list of users */
typedef std::map<TeamPtr, UsersVector> Team2Users;

/** Add relations user to his team for the competition to the map */
void tcm_map_team_to_users(Team2Users& result, const CompetitionPtr& c);

/** Add the member of the team to the competition */
void tcm_add(const TeamPtr& team, const CompetitionPtr& competition,
             const UserPtr& user);

/** Remove the member of the team to the competition */
void tcm_remove(const CompetitionPtr& competition, const UserPtr& user);

}

DBO_EXTERN_TEMPLATES(thechess::TCM);

#endif

