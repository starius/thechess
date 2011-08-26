/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WBreak>
#include <Wt/WPushButton>

#include "widgets/PleaseLoginWidget.hpp"
#include "widgets/LoginWidget.hpp"
#include "widgets/RegisterWidget.hpp"

namespace thechess {
namespace widgets {

PleaseLoginWidget::PleaseLoginWidget(Wt::WContainerWidget* parent) :
    Wt::WContainerWidget(parent) {
    new Wt::WText(tr("tc.user.Please_login"), this);
    new Wt::WBreak(this);
    Wt::WPushButton* login = new Wt::WPushButton(tr("tc.user.Login"), this);
    login->clicked().connect(this, &PleaseLoginWidget::login_);
    Wt::WPushButton* reg = new Wt::WPushButton(tr("tc.user.Register"), this);
    reg->clicked().connect(this, &PleaseLoginWidget::register_);
}

void PleaseLoginWidget::login_() {
    clear();
    new LoginWidget(this);
}

void PleaseLoginWidget::register_() {
    clear();
    new RegisterWidget(this);
}

}
}
