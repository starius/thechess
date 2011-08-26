/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_OBJECT_HPP_
#define THECHESS_MODEL_OBJECT_HPP_

#include <vector>

#include <Wt/Dbo/Session>
#include <Wt/WDateTime>

namespace dbo = Wt::Dbo;

namespace thechess {
namespace model {

struct Object;
typedef std::vector<Object> Objects;

enum ObjectType {
    NoEvent,
    GameObject,
    UserObject,
    CompetitionObject
}; // change Object::reread() after changing this

struct Object {
    Object(ObjectType ot, int i);

    ObjectType type;
    int id;

    bool operator<(const Object& b) const {
        return id < b.id || (id == b.id && type < b.type);
    }

    bool operator==(const Object& b) const {
        return id == b.id && type == b.type;
    }

    void reread(dbo::Session& session) const;
    Wt::WDateTime process(Objects& objects, dbo::Session& session) const;
};

}
}

#endif

