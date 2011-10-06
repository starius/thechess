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

struct Object;
typedef std::vector<Object> Objects;

/** Model type */
enum ObjectType {
    NOEVENT,
    GAME,
    USER,
    COMPETITION
}; // change Object::reread() after changing this

/** Struct referencing any instance of any changable model.

\ingroup model
*/
struct Object {
    /** Constructor */
    Object(ObjectType ot, int i);

    /** Model type */
    ObjectType type;

    /** Database id of instance */
    int id;

    /** Comparison operator */
    bool operator<(const Object& b) const {
        return id < b.id || (id == b.id && type < b.type);
    }

    /** Comparison operator */
    bool operator==(const Object& b) const {
        return id == b.id && type == b.type;
    }

    /** Reread to the session */
    void reread(dbo::Session& session) const;

    /** Trigger processing methods, return datetime of next trigger */
    Wt::WDateTime process(Objects& objects, dbo::Session& session) const;
};

}

#endif

