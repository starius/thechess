/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include "model/all.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::TCM);

namespace thechess {

TCMId::TCMId()
{ }

TCMId::TCMId(const CompetitionPtr& c, const UserPtr& u):
    competition(c), user(u)
{ }

bool TCMId::operator==(const TCMId& other) const {
    return competition == other.competition && user == other.user;
}

bool TCMId::operator!=(const TCMId& other) const {
    return !(*this == other);
}

bool TCMId::operator<(const TCMId& other) const {
    return competition < other.competition ||
           (competition == other.competition && user < other.user);
}

std::ostream& operator<<(std::ostream& o, const TCMId& id) {
    o << '(' << id.competition << ',' << id.user << ')';
    return o;
}

TCM::TCM()
{ }

TCM::TCM(const TeamPtr& t, const CompetitionPtr& c, const UserPtr& u):
    id_(c, u), team_(t)
{ }

void tcm_map_user_to_team(User2Team& result,
                          const CompetitionPtr& competition) {
    TCMs tsms = competition.session()->find<TCM>()
                .where("competition_id = ?").bind(competition);
    BOOST_FOREACH (TCMPtr tcm, tsms) {
        const UserPtr& user = tcm->user();
        const TeamPtr& team = tcm->team();
        result[user] = team;
    }
}

void tcm_map_team_to_users(Team2Users& result, const CompetitionPtr& c) {
    TCMs tsms = c.session()->find<TCM>()
                .where("competition_id = ?").bind(c);
    BOOST_FOREACH (TCMPtr tcm, tsms) {
        const UserPtr& user = tcm->user();
        const TeamPtr& team = tcm->team();
        result[team].push_back(user);
    }
}

void tcm_add(const TeamPtr& team, const CompetitionPtr& competition,
             const UserPtr& user) {
    competition.session()->add(new TCM(team, competition, user));
}

void tcm_remove(const CompetitionPtr& competition, const UserPtr& user) {
    dbo::Session& s = *competition.session();
    TCMPtr tcm = s.load<TCM>(TCMId(competition, user));
    tcm.remove();
}

}

