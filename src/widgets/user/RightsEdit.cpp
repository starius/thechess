/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <Wt/WCheckBox>
#include <Wt/WPushButton>

#include "widgets/user/RightsEdit.hpp"
#include "Application.hpp"
#include "log.hpp"

namespace thechess {

RightsEdit::RightsEdit(const UserPtr& user, Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    user_(user) {
    dbo::Transaction t(tApp->session());
    if (!tApp->user()) {
        return;
    }
    User::for_each_right(boost::bind(&RightsEdit::add_item, this, _1));
    Wt::WPushButton* save = new Wt::WPushButton(tr("tc.common.Save"), this);
    save->clicked().connect(this, &RightsEdit::save);
}

void RightsEdit::add_item(User::Rights right) {
    if (user_->can_change_right(right, tApp->user())) {
        Wt::WCheckBox* box = new Wt::WCheckBox(tr(User::right_to_str(right)));
        box->setChecked(user_->has_permission(right));
        items_[right] = box;
        addWidget(box);
        new Wt::WBreak(this);
    }
}

void RightsEdit::save() {
    dbo::Transaction t(tApp->session());
    user_.reread();
    BOOST_FOREACH (const Items::value_type& it, items_) {
        User::Rights right = it.first;
        Wt::WCheckBox* box = it.second;
        user_.modify()->set_permission(right, box->isChecked());
    }
    admin_log("Change rights of " + user_a(user_.id()));
    t.commit();
    tApp->path().open(tApp->internalPath());
}

}

