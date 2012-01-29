/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/all.hpp"

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

}

