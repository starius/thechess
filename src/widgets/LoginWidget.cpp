/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WLabel>
#include <Wt/WTable>
#include <Wt/WRegExpValidator>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "ThechessApplication.hpp"
#include "widgets/LoginWidget.hpp"
#include "config.hpp"
#include "model/all.hpp"

namespace thechess {

LoginWidget::LoginWidget(Wt::WContainerWidget* parent) :
    Wt::WContainerWidget(parent) {
    table_ = new Wt::WTable(this);
    Wt::WLabel* username_label = new Wt::WLabel(tr("tc.user.Username"),
            table_->elementAt(0, 0));
    username_ = new Wt::WLineEdit(table_->elementAt(0, 1));
    Wt::WLabel* password_label = new Wt::WLabel(tr("tc.user.Password"),
            table_->elementAt(1, 0));
    password_ = new Wt::WLineEdit(table_->elementAt(1, 1));
    password_->setEchoMode(Wt::WLineEdit::Password);
    username_label->setBuddy(username_);
    password_label->setBuddy(password_);
    error_ = new Wt::WText(this);
    button_ = new Wt::WPushButton(tr("tc.user.Login"), this);
    button_->clicked().connect(this, &LoginWidget::try_);
}

void LoginWidget::try_() {
    error_->setText(tr(""));
    dbo::Transaction t(tApp->session());
    UserPtr user = tApp->session().find<User>()
                   .where("username = ?")
                   .bind(username_->text());
    if (!user || !user->test_password(password_->text().toUTF8())) {
        error_->setText(tr("tc.user.Wrong_username_or_password"));
    } else {
        tApp->set_user(user);
    }
    t.commit();
}

}

