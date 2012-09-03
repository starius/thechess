/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Auth/AuthModel>
#include <Wt/Auth/RegistrationModel>

#include "widgets/user/AuthWidget.hpp"
#include "Application.hpp"

namespace thechess {

AuthWidget::AuthWidget(Wt::WContainerWidget* parent):
    Wt::Auth::AuthWidget(
#if WT_MINOR==0x0
        Server::instance()->auth_service(),
        tApp->session().user_database(),
#endif
        tApp->session().login(),
        parent) {
    using namespace Wt::Auth;
#if WT_MINOR==0x0
    addPasswordAuth(&Server::instance()->password_service());
#else
    AuthModel* m = new AuthModel(Server::instance()->auth_service(),
                                 tApp->session().user_database(), this);
    m->addPasswordAuth(&Server::instance()->password_service());
    setModel(m);
#endif
    setRegistrationEnabled(true);
    try {
        processEnvironment();
    } catch (std::exception& e) {
        tApp->log("warning") << "AuthWidget.processEnvironment(): " << e.what();
    }
}

Wt::Auth::RegistrationModel* AuthWidget::createRegistrationModel() {
    Wt::Auth::RegistrationModel* result;
    result = Wt::Auth::AuthWidget::createRegistrationModel();
    result->setEmailPolicy(Wt::Auth::RegistrationModel::EmailMandatory);
    return result;
}

}

