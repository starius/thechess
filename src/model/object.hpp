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
#include "thechess-global.hpp"

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

/** Task which will re-add itself to planning server on exception thrown */
struct TryAgainTask : public Task {
    /** Apply the action.
    Calls process_impl(task).
    On exception (except non-found and unknown) re-adds itself to planning
    in random duration.
    */
    void process(TaskPtr task, Planning* server) const;

    /** Get event key (dummy) */
    std::string key() const;

    /** Apply the action (implementation).
    This method could throw exceptions.
    Use session passed acess database objects.
    Transaction is already opened.
    */
    virtual void process_impl(TaskPtr task, Session& session) const = 0;
};

/** Struct referencing any instance of any changable model.

\ingroup model
*/
struct Object : public TryAgainTask {
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

    /** Run checks for planned task */
    void process_impl(TaskPtr task, Session& session) const;

    /** Get event key */
    std::string key() const;
};

/** Event emited when private messge is sent */
class NewMessage : public Event {
public:
    /** Constructor */
    NewMessage(int reader_id, int sender_id = -1);

    /** ID of addressee User */
    int reader_id;

    /** ID of author User */
    int sender_id;

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

/** Notify listening widgets through task.
Use this function instead of t_emit to notify about
changed objects from process().
*/
void t_emit_after(ObjectType type, int id, const Wt::WDateTime& when);

/** Notify listening widgets through task */
void t_emit_after(ObjectType type, int id);

/** Notify listening widgets */
void t_emit(const std::string& key);

/** Add task to planning server (when = now()) */
void t_task(TaskPtr task);

/** Add task to planning server (when = now()) */
void t_task(Task* task);

/** Add task to planning server */
void t_task(TaskPtr task, const Wt::WDateTime& when);

/** Add task to planning server */
void t_task(Task* task, const Wt::WDateTime& when);

/** Add task to planning server (when = now()) */
void t_task(ObjectType type, int id);

/** Add task to planning server */
void t_task(ObjectType type, int id, const Wt::WDateTime& when);

}

#endif

