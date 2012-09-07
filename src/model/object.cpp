/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>

#include <Wt/Dbo/Transaction>
#include <Wt/Wc/util.hpp>

#include "model/all.hpp"
#include "Session.hpp"
#include "Planning.hpp"
#include "Server.hpp"
#include "Application.hpp" // FIXME
#include "config.hpp"

namespace thechess {

Object::Object(ObjectType ot, int i) :
    type(ot), id(i) {
    user_id = !tApp ? 0 : !tApp->user() ? 0 : tApp->user().id();
}

Td rand_td(const std::pair<Td, Td>& range) {
    return rand_range(range.first, range.second);
}

void Object::process(Wt::Wc::notify::TaskPtr task,
                     Wt::Wc::notify::PlanningServer* server) const {
    std::cerr << "Check object: " << key() << std::endl;
    Planning* planning = downcast<Planning*>(server);
    Session session(planning->server().pool());
    try {
        dbo::Transaction t(session);
        Wt::WDateTime result;
        if (type == GAME) {
            GamePtr game = session.load<Game>(id, /* reread */ true);
            game.modify()->check(task, planning);
        }
        if (type == COMPETITION) {
            CompetitionPtr c = session.load<Competition>(id, /* reread */ true);
            c.modify()->check(task, planning);
        }
        // FIXME: USER, COMPETITION
        t.commit();
    } catch (dbo::ObjectNotFoundException e) {
        std::cerr << e.what() << std::endl;
    } catch (dbo::StaleObjectException& e) {
        std::cerr << e.what() << std::endl;
        Td delay = rand_td(config::tracker::STALE_OBJECT_DELAY);
        planning->add(task, now() + delay, false);
    } catch (std::exception& e) { // database locked?
        std::cerr << e.what() << std::endl;
        Td delay = rand_td(config::tracker::UNKNOWN_ERROR_DELAY);
        planning->add(task, now() + delay, false);
    } catch (...)
    { }
}

std::string Object::key() const {
    return boost::lexical_cast<std::string>(type) + "-" +
           boost::lexical_cast<std::string>(id);
}

NewMessage::NewMessage(int r):
    reader_id(r)
{ }

std::string NewMessage::key() const {
    return TO_S(reader_id);
}

}

