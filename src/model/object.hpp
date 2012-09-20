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

/** Notifier class */
typedef Wt::Wc::notify::Server Notifier;

/** Planning class */
typedef Wt::Wc::notify::PlanningServer Planning;

/** Event for notifications */
typedef Wt::Wc::notify::Event Event;

/** Shared pointer to Event */
typedef Wt::Wc::notify::EventPtr EventPtr;

/** Task for planning */
typedef Wt::Wc::notify::Task Task;

/** Shared pointer to Task */
typedef Wt::Wc::notify::TaskPtr TaskPtr;

/** Struct referencing any instance of any changable model.

\ingroup model
*/
struct Object : public Task {
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
    void process(TaskPtr task, Planning* server) const;

    /** Get event key */
    std::string key() const;
};

/** Event emited when private messge is sent */
class NewMessage : public Event {
public:
    /** Constructor */
    NewMessage(int reader_id);

    /** ID of addressee User */
    int reader_id;

    /** Comparison operator */
    bool operator<(const NewMessage& b) const {
        return reader_id < b.reader_id;
    }

    /** Comparison operator */
    bool operator==(const NewMessage& b) const {
        return reader_id == b.reader_id;
    }

    /** Get event key */
    std::string key() const;
};

/** Notifiable widget */
class Notifiable : public Wt::Wc::notify::Widget {
public:
    /** Constructor */
    Notifiable(const std::string& key);
};

/** Notify listening widgets */
void t_emit(EventPtr event);

/** Notify listening widgets */
void t_emit(Event* event);

/** Notify listening widgets */
void t_emit(ObjectType type, int id);

/** Notify listening widgets */
void t_emit(const std::string& key);

/** Add task to planning server (when = now()) */
void t_task(TaskPtr task);

/** Add task to planning server */
void t_task(TaskPtr task, const Wt::WDateTime& when);

/** Add task to planning server (when = now()) */
void t_task(ObjectType type, int id);

/** Add task to planning server */
void t_task(ObjectType type, int id, const Wt::WDateTime& when);

}

#endif

