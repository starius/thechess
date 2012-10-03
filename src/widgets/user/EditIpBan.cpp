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
#include "Application.hpp"
#include "log.hpp"

namespace thechess {

EditIpBan::EditIpBan(const IpBanPtr& ban, Wt::WContainerWidget* parent):
    WContainerWidget(parent), ban_(ban) {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    Wt::WPushButton* b = new Wt::WPushButton(this);
    b->clicked().connect(this, &EditIpBan::save);
    if (ban_->enabled()) {
        b->setText(tr("tc.common.Discard"));
    } else {
        b->setText(tr("tc.common.Confirm"));
    }
}

void EditIpBan::save() {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    try {
        ban_.modify()->set_enabled(!ban_->enabled());
        admin_log("Change ban of # " + TO_S(ban_.id()) + " of " + ban_->ip());
        t.commit();
        delete this; // TODO go to page with ban list
    } catch (dbo::StaleObjectException e) {
        ban_.reread();
        wApp->setInternalPath(wApp->internalPath(), /* emit */ true);
    }
}

}

