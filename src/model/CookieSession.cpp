/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>

#include "model/CookieSession.hpp"

namespace thechess {

CookieSession::CookieSession() {
}

CookieSession::CookieSession(const std::string& cookie_id) :
    cookie_id_(cookie_id) {
}

}

