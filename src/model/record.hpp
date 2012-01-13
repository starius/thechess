/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_RECORD_HPP_
#define THECHESS_MODEL_RECORD_HPP_

#include "model/global.hpp"

namespace thechess {

/** A class with standard fields for a model, created by a user.

Add Many-to-One relation from User model to a model, inherited from this class,
with join id "init".

\ingroup model
*/
class Record {
public:
    /** Constructor */
    Record(const UserPtr& init = UserPtr(), Wt::WString name = "",
           Wt::WString descr = "");

    /** Persist method (see Wt::Dbo) */
    template<class Action>
    void persist(Action& a) {
        dbo::field(a, name_, "name");
        dbo::field(a, description_, "description");
        dbo::belongsTo(a, init_, "init");
        dbo::field(a, created_, "created");
    }

    /** Get name */
    const Wt::WString& name() const {
        return name_;
    }

    /** Set name */
    void set_name(const Wt::WString& v) {
        name_ = v;
    }

    /** Get description */
    const Wt::WString& description() const {
        return description_;
    }

    /** Set description */
    void set_description(const Wt::WString& v) {
        description_ = v;
    }

    /** Get creation datetime */
    const Wt::WDateTime& created() const {
        return created_;
    }

    /** Get the creator */
    const UserPtr& init() const {
        return init_;
    }

    /** Set the creator */
    void set_init(const UserPtr& init) {
        init_ = init;
    }

private:
    Wt::WString name_;
    Wt::WString description_;
    UserPtr init_;
    Wt::WDateTime created_;
};

}

#endif

