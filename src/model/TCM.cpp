/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/all.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::TCM);

namespace thechess {

TCMId::TCMId()
{ }

TCMId::TCMId(const TeamPtr& t, const CompetitionPtr& c, const UserPtr& u):
    team(t), competition(c), user(u)
{ }

bool TCMId::operator==(const TCMId& other) const {
    return team == other.team && competition == other.competition &&
           user == other.user;
}

bool TCMId::operator!=(const TCMId& other) const {
    return !(*this == other);
}

bool TCMId::operator<(const TCMId& other) const {
    return team < other.team ||
           (team == other.team && (competition < other.competition ||
                                   (competition == other.competition &&
                                    user < other.user)));
}

std::ostream& operator<<(std::ostream& o, const TCMId& id) {
    o << '(' << id.team << ',' << id.competition << ',' << id.user << ')';
    return o;
}

TCM::TCM()
{ }

TCM::TCM(const TeamPtr& t, const CompetitionPtr& c, const UserPtr& u):
    id_(t, c, u)
{ }

}

