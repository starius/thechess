/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/Dbo/Transaction>
#include <Wt/Wc/util.hpp>

#include "model/all.hpp"
#include "Session.hpp"
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

void TryAgainTask::process(TaskPtr task, Planning* server) const {
    Session session(Server::instance()->pool());
    try {
        dbo::Transaction t(session);
        process_impl(task, session);
        t.commit();
    } catch (dbo::ObjectNotFoundException e) {
        std::cerr << e.what() << std::endl;
    } catch (dbo::StaleObjectException& e) {
        std::cerr << e.what() << std::endl;
        Td delay = rand_td(config::tracker::STALE_OBJECT_DELAY);
        t_task(task, now() + delay);
    } catch (std::exception& e) { // database locked?
        std::cerr << e.what() << std::endl;
        Td delay = rand_td(config::tracker::UNKNOWN_ERROR_DELAY);
        t_task(task, now() + delay);
    } catch (...)
    { }
}

std::string TryAgainTask::key() const {
    return "";
}

void Object::process_impl(TaskPtr task, Session& session) const {
    std::cerr << "Check object: " << key() << std::endl;
    if (type == GAME) {
        GamePtr game = session.load<Game>(id, /* reread */ true);
        game.modify()->check(task);
    }
    if (type == COMPETITION) {
        CompetitionPtr c = session.load<Competition>(id, /* reread */ true);
        c.modify()->check(task);
    }
    if (type == USER) {
        UserPtr user = session.load<User>(id, /* reread */ true);
        user.modify()->check(task);
    }
    if (type == IP_BAN) {
        IpBanPtr ip_ban = session.load<IpBan>(id, /* reread */ true);
        ip_ban.modify()->check(task);
    }
}

std::string Object::key() const {
    return boost::lexical_cast<std::string>(type) + "-" +
           boost::lexical_cast<std::string>(id);
}

NotifyTask::NotifyTask(ObjectType ot, int i):
    Object(ot, i)
{ }

void NotifyTask::process(TaskPtr task, Planning* server) const
{ }

NewMessage::NewMessage(int r):
    reader_id(r)
{ }

std::string NewMessage::key() const {
    return TO_S(reader_id);
}

Notifier* t_notifier() {
    return &Server::instance()->notifier_;
}

Notifiable::Notifiable(const std::string& key):
    Wt::Wc::notify::Widget(key, t_notifier())
{ }

void t_emit(EventPtr event) {
    t_notifier()->emit(event);
}

void t_emit(Event* event) {
    t_notifier()->emit(event);
}

void t_emit(ObjectType type, int id) {
    t_notifier()->emit(boost::make_shared<Object>(type, id));
}

void t_emit(const std::string& key) {
    t_notifier()->emit(key);
}

Planning* t_planning() {
    return &Server::instance()->planning_;
}

void t_task(TaskPtr task, const Wt::WDateTime& when) {
    std::cerr << "t_task " << task->key() + " " << when.toString() << std::endl;
    t_planning()->add(task, when);
    if (wApp && !wApp->environment().ajax()) {
        t_emit(task);
    }
}

void t_task(Task* task, const Wt::WDateTime& when) {
    t_task(TaskPtr(task), now());
}

void t_task(TaskPtr task) {
    t_task(task, now());
}

void t_task(Task* task) {
    t_task(TaskPtr(task), now());
}

void t_task(ObjectType type, int id, const Wt::WDateTime& when) {
    t_task(boost::make_shared<Object>(type, id), when);
}

void t_task(ObjectType type, int id) {
    t_task(type, id, now());
}

void t_emit_after(ObjectType type, int id, const Wt::WDateTime& when) {
    t_task(boost::make_shared<NotifyTask>(type, id), when);
}

void t_emit_after(ObjectType type, int id) {
    t_task(boost::make_shared<NotifyTask>(type, id), now());
}

}

