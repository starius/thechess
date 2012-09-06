/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

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

bool IpBan::is_banned(const std::string& ip) {
    if (!tApp) {
        return false;
    }
    dbo::Transaction t(tApp->session());
    IpBans bans = tApp->session().find<IpBan>()
                  .where("ip = ?").bind(ip)
                  .where("enabled = ?").bind(true)
                  .resultList();
    BOOST_FOREACH (IpBanPtr ban, bans) {
        if (ban->start() <= now() && now() <= ban->stop()) {
            return true;
        }
    }
    return false;
}

bool IpBan::am_i_banned() {
    if (!tApp) {
        return false;
    }
    std::string ip = wApp->environment().clientAddress();
    return is_banned(ip);
}

}
