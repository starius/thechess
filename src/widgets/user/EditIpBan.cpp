/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/Wc/util.hpp>

#include "widgets/user/EditIpBan.hpp"
#include "widgets/user/user_anchor.hpp"
#include "Application.hpp"
#include "log.hpp"

namespace thechess {

EditIpBan::EditIpBan(const IpBanPtr& ban, Wt::WContainerWidget* parent):
    WContainerWidget(parent), ban_(ban) {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    new Wt::WText(ban_->ip(), this);
    new Wt::WBreak(this);
    new Wt::WText(ban_->reason(), this);
    new Wt::WBreak(this);
    new Wt::WText(ban_->stop().toString(), this);
    new Wt::WBreak(this);
    user_anchor(ban_->creator(), this);
    new Wt::WBreak(this);
    button(BAN_DISABLED);
    button(LIMITED_NEW);
    button(NO_REGISTRATION);
    button(ABSOLUTE_BAN);
}

void EditIpBan::button(BanState state) {
    Wt::WPushButton* b = new Wt::WPushButton(this);
    b->clicked().connect(boost::bind(&EditIpBan::save, this, state));
    b->setText(tr(IpBan::state2str(state)));
}

void EditIpBan::save(BanState state) {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    try {
        ban_.modify()->set_state(state);
        admin_log("Change ban of # " + TO_S(ban_.id()) + " of " + ban_->ip());
        t.commit();
        t_task(IP_BAN, ban_.id());
        delete this; // TODO go to page with ban list
    } catch (dbo::StaleObjectException e) {
        ban_.reread();
        wApp->setInternalPath(wApp->internalPath(), /* emit */ true);
    }
}

}

