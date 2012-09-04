/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "model/all.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::IpBan);

namespace thechess {

IpBan::IpBan()
{ }

IpBan::IpBan(bool):
    enabled_(true), start_(now()) {
    if (wApp) {
        ip_ = wApp->environment().clientAddress();
    }
}

}

