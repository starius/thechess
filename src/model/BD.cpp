/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/Wc/Gather.hpp>

#include "model/all.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::BD);

namespace thechess {

BDId::BDId()
{ }

BDId::BDId(const UserPtr& u, Wt::Wc::Gather::DataType t, const std::string& v):
    user(u), type(t), value(v)
{ }

bool BDId::operator==(const BDId& other) const {
    return user == other.user && type == other.type && value == other.value;
}

bool BDId::operator<(const BDId& other) const {
    return user < other.user || (user == other.user && (type < other.type ||
                                 (type == other.type && value < other.value)));
}

std::ostream& operator<<(std::ostream& o, const BDId& id) {
    return o << '(' << id.user << ',' << id.type << ',' << id.value << ')';
}

BD::BD()
{ }

BD::BD(const BDId& id):
    id_(id), used_(now())
{ }

const UserPtr& BD::user() const {
    return id_.user;
}

Wt::Wc::Gather::DataType BD::type() const {
    return id_.type;
}

const std::string& BD::value() const {
    return id_.value;
}

void BD::use() {
    used_ = now();
}

dbo::Query<BD::BDPair> BD::pairs(dbo::Session& session) {
    return session.query<BDPair>(
               "select U, V "
               "from thechess_bd U "
               "inner join thechess_bd V on U.type=V.type and U.value=V.value ")
           .where("U.user_id<V.user_id");
}

void BD::scores(const dbo::Query<BD::BDPair>& pairs_q, BD::Scores& scores) {
    dbo::collection<BDPair> pairs = pairs_q;
    BOOST_FOREACH (BDPair pair, pairs) {
        UserPtr a = pair.get<0>()->user();
        UserPtr b = pair.get<1>()->user();
        Wt::Wc::Gather::DataType type = pair.get<0>()->type();
        int score = Wt::Wc::Gather::significance(type);
        scores[UserPair(a, b)] += score;
    }
}

void BD::filter(BD::Scores& scores, int min_score) {
    if (min_score == -1) {
        min_score = Wt::Wc::Gather::MIN_SIGNIFICANT;
    }
    std::set<UserPair> to_remove;
    BOOST_FOREACH (const Scores::value_type& pair_and_score, scores) {
        int score = pair_and_score.second;
        const UserPair& pair = pair_and_score.first;
        if (score < min_score) {
            to_remove.insert(pair);
        }
    }
    BOOST_FOREACH (const UserPair& pair, to_remove) {
        scores.erase(pair);
    }
}

void BD::add_users(const BD::Scores& scores, std::set<UserPtr>& users) {
    BOOST_FOREACH (const Scores::value_type& pair_and_score, scores) {
        const UserPair& pair = pair_and_score.first;
        const UserPtr a = pair.first();
        const UserPtr b = pair.second();
        users.insert(a);
        users.insert(b);
    }
}

}

