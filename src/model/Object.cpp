/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>

#include <Wt/Dbo/Transaction>

#include "model/Object.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"
#include "model/Competition.hpp"
#include "config.hpp"

namespace thechess {
namespace model {

Object::Object(ObjectType ot, int i) :
    type(ot), id(i) {
}

void Object::reread(dbo::Session& session) const {
    dbo::Transaction t(session);
    if (type == GameObject) {
        session.load<Game>(id).reread();
    }
    if (type == UserObject) {
        session.load<User>(id).reread();
    }
    if (type == CompetitionObject) {
        session.load<Competition>(id).reread();
    }
    t.commit();
}

Wt::WDateTime Object::process(Objects& objects, dbo::Session& session) const {
    Wt::WDateTime result;
    if (type == GameObject) {
        GamePtr game = session.load<Game>(id);
        game.reread();
        game.modify()->check(objects);
        result = game->next_check();
    }
    if (type == CompetitionObject) {
        CompetitionPtr c = session.load<Competition>(id);
        c.reread();
        c.modify()->check(objects);
        result = c->next_check();
    }
    // FIXME: UserObject, CompetitionObject
    if (result.isValid()) {
        result += config::tracker::delay;
    }
    return result;
}

}
}

