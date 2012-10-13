/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <algorithm>
#include <boost/foreach.hpp>

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "model/all.hpp"
#include "Application.hpp"
#include "Options.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::IpBan);

namespace thechess {

IpBan::IpBan()
{ }

IpBan::IpBan(bool):
    start_(now()), state_(LIMITED_NEW)
{ }

const char* IpBan::state2str(BanState state) {
    if (state == BAN_DISABLED) {
        return "tc.user.Ban_disabled";
    } else if (state == LIMITED_NEW) {
        return "tc.user.Limited_new";
    } else if (state == NO_REGISTRATION) {
        return "tc.user.No_registration";
    } else if (state == ABSOLUTE_BAN) {
        return "tc.user.Absolute_ban";
    } else {
        return "";
    }
}

BanState IpBan::is_banned(const std::string& ip) {
    if (!tApp) {
        return BAN_DISABLED;
    }
    dbo::Transaction t(tApp->session());
    const char* where_ip = Options::instance()->database_type() == POSTGRES ?
                           "inet(text(?)) <<= inet(ip)" : "ip = ?";
    IpBans bans = tApp->session().find<IpBan>()
                  .where(where_ip).bind(ip)
                  .where("state <> ?").bind(BAN_DISABLED)
                  .resultList();
    BanState result = BAN_DISABLED;
    BOOST_FOREACH (IpBanPtr ban, bans) {
        if (ban->start() <= now() && now() <= ban->stop()) {
            result = std::max(result, ban->state());
        }
    }
    return result;
}

BanState IpBan::am_i_banned() {
    if (!tApp) {
        return BAN_DISABLED;
    }
    std::string ip = wApp->environment().clientAddress();
    return is_banned(ip);
}

void IpBan::check(Wt::Wc::notify::TaskPtr task) {
    if (now() > stop()) {
        set_state(BAN_DISABLED);
    } else if (state()) {
        t_task(task, stop());
    }
}

}

