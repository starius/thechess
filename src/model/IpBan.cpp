/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "model/all.hpp"
#include "Application.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::IpBan);

namespace thechess {

IpBan::IpBan()
{ }

IpBan::IpBan(bool):
    enabled_(true), start_(now())
{ }

bool IpBan::i_am_banned() {
    if (!tApp) {
        return false;
    }
    dbo::Transaction t(tApp->session());
    std::string ip = wApp->environment().clientAddress();
    return tApp->session().find<IpBan>()
           .where("ip = ?").bind(ip)
           .where("enabled = ?").bind(true)
           .where("start < ?").bind(now())
           .where("stop > ?").bind(now())
           .limit(1)
           .resultList().size();
}

}

