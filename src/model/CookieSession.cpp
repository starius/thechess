/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/all.hpp"

namespace thechess {

CookieSession::CookieSession() {
}

CookieSession::CookieSession(const std::string& cookie_id) :
    cookie_id_(cookie_id) {
}

}

