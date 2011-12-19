/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COOKIESESSION_HPP_
#define THECHESS_MODEL_COOKIESESSION_HPP_

#include "model/global.hpp"
#include "config.hpp"

namespace thechess {

/** Model representing persisted user log-in.
This model includes session-in, stored at client-side in cookies.

\ingroup model
*/
class CookieSession {
public:

#ifndef DOXYGEN_ONLY
    CookieSession();
#endif

    /** Create instance to be added to database */
    CookieSession(const std::string& cookie_id);

#ifndef DOXYGEN_ONLY
    template<class Action>
    void persist(Action& a) {
        dbo::belongsTo(a, user_, "user");
        dbo::id(a, cookie_id_, "cookie_id");
        dbo::field(a, used_, "used");
    }
#endif

    /** Get session id, stored in cookies */
    const std::string& cookie_id() const {
        return cookie_id_;
    }

    /** Get user */
    const UserPtr& user() const {
        return user_;
    }

    /** Set user */
    void set_user(const UserPtr& user) {
        user_ = user;
    }

    /** Run this method when it is used (by user) */
    void use() {
        used_ = Wt::WDateTime::currentDateTime();
    }

    /** Get datetime when it was used last time */
    const Wt::WDateTime& used() const {
        return used_;
    }

private:
    UserPtr user_;
    std::string cookie_id_;
    Wt::WDateTime used_;
};

}

#endif

