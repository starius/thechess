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
#include "widgets/RegisterWidget.hpp"
#include "config.hpp"
#include "model/User.hpp"

namespace thechess {
namespace widgets {
using namespace model;

RegisterWidget::RegisterWidget(Wt::WContainerWidget* parent) :
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
    Wt::WRegExpValidator* username_validator = new Wt::WRegExpValidator();
    username_validator->setRegExp(thechess::config::username_regex);
    username_validator->setMandatory(true);
    username_->setValidator(username_validator);
    Wt::WRegExpValidator* password_validator = new Wt::WRegExpValidator();
    password_validator->setRegExp(thechess::config::password_regex);
    password_validator->setMandatory(true);
    password_->setValidator(password_validator);
    username_error_ = new Wt::WText(table_->elementAt(0, 2));
    password_error_ = new Wt::WText(table_->elementAt(1, 2));
    button_ = new Wt::WPushButton(tr("tc.user.Register"), this);
    button_->clicked().connect(this, &RegisterWidget::try_register_);
}

void RegisterWidget::try_register_() {
    username_error_->setText(tr(""));
    Wt::WValidator::State Valid = Wt::WValidator::Valid;
    bool ok = true;
    if (username_->validate() != Valid) {
        ok = false;
    }
    if (password_->validate() != Valid) {
        ok = false;
    }
    if (ok) {
        dbo::Transaction t(tApp->session());
        UserPtr check_username = tApp->session().find<User>()
                                 .where("username = ?")
                                 .bind(username_->text());
        if (check_username) {
            username_error_->setText(tr("thechess.already_used"));
        } else {
            User* u = new User(true);
            u->set_username(username_->text());
            u->set_password(password_->text().toUTF8());
            UserPtr user = tApp->session().add(u);
            table_->hide();
            button_->hide();
            new Wt::WText(tr("thechess.register_success"), this);
            tApp->set_user(user);
        }
        t.commit();
    }
}

}
}
