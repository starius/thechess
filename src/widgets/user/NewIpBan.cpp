/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTextArea>
#include <Wt/WLineEdit>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/Wc/TableForm.hpp>
#include <Wt/Wc/TimeDurationWidget.hpp>

#include "widgets/user/NewIpBan.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "log.hpp"

namespace thechess {

NewIpBan::NewIpBan(const std::string& ip, const Wt::WString& reason,
                   Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    if (IpBan::is_banned(ip)) {
        addWidget(new Wt::WText(tr("tc.user.Already_banned")));
        return;
    }
    Wt::Wc::TableForm* f = new Wt::Wc::TableForm(this);
    ip_ = new Wt::WLineEdit(Wt::WString::fromUTF8(ip));
    f->item(tr("tc.user.ip"), "", 0, ip_);
    duration_ = new Wt::Wc::TimeDurationWidget(MINUTE, 3 * DAY, 999 * WEEK);
    f->item(tr("tc.common.duration"), "", duration_->form_widget(), duration_);
    reason_ = new Wt::WTextArea(reason);
    f->item(tr("tc.user.ban_reason"), "", reason_, reason_);
    Wt::WPushButton* save = new Wt::WPushButton(tr("tc.common.Save"), this);
    save->clicked().connect(this, &NewIpBan::add);
}

void NewIpBan::add() {
    dbo::Transaction t(tApp->session());
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    IpBanPtr ban = tApp->session().add(new IpBan(true));
    std::string ip = ip_->text().toUTF8();
    ban.modify()->set_ip(ip);
    ban.modify()->set_start(now());
    ban.modify()->set_stop(now() + duration_->corrected_value());
    ban.modify()->set_reason(reason_->text());
    ban.modify()->set_creator(tApp->user());
    admin_log("Ban " + ip_a(ip));
    t.commit();
    t_task(IP_BAN, ban.id());
    delete this; // TODO go to page with ban list
}

}

