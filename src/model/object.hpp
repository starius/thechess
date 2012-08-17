/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_OBJECT_HPP_
#define THECHESS_MODEL_OBJECT_HPP_

#include <vector>

#include <Wt/Dbo/Session>
#include <Wt/WDateTime>
#include <Wt/Wc/Notify.hpp>
#include <Wt/Wc/Planning.hpp>

#include "model/global.hpp"

namespace dbo = Wt::Dbo;

namespace thechess {

/** Struct referencing any instance of any changable model.

\ingroup model
*/
struct Object : public Wt::Wc::notify::Task {
    /** Constructor */
    Object(ObjectType ot, int i);

    /** Model type */
    ObjectType type;

    /** Database id of instance */
    int id;

    /** Id of user, emitted this object.
    This field is set automatically.
    */
    int user_id;

    /** Comparison operator */
    bool operator<(const Object& b) const {
        return id < b.id || (id == b.id && type < b.type);
    }

    /** Comparison operator */
    bool operator==(const Object& b) const {
        return id == b.id && type == b.type;
    }

    /** Run checks for planned task */
    void process(Wt::Wc::notify::TaskPtr task,
                 Wt::Wc::notify::PlanningServer* server) const;

    /** Get event key */
    std::string key() const;
};

}

#endif

