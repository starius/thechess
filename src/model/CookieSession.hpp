/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COOKIESESSION_HPP_
#define THECHESS_MODEL_COOKIESESSION_HPP_

#include "model/global.hpp"
#include "config.hpp"

namespace thechess {

class CookieSession {
public:
    CookieSession();
    CookieSession(const std::string& cookie_id);

    template<class Action>
    void persist(Action& a) {
        dbo::belongsTo(a, user_, "user");
        dbo::id(a, cookie_id_, "cookie_id");
        dbo::field(a, used_, "used");
    }

    const std::string& cookie_id() const {
        return cookie_id_;
    }

    const UserPtr& user() const {
        return user_;
    }
    void set_user(const UserPtr& user) {
        user_ = user;
    }

    void use() {
        used_ = Wt::WDateTime::currentDateTime();
    }
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

