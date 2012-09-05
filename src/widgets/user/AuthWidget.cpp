/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/Auth/AuthModel>
#include <Wt/Auth/RegistrationModel>
#include <Wt/Auth/RegistrationWidget>
#include <Wt/Wc/rand.hpp>

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
        parent),
    model_(0) {
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

class RegistrationModel : public Wt::Auth::RegistrationModel {
public:
    RegistrationModel(Wt::WObject* parent = 0):
        Wt::Auth::RegistrationModel(Server::instance()->auth_service(),
                                    tApp->session().user_database(),
                                    tApp->session().login(),
                                    this) {
        addPasswordAuth(&Server::instance()->password_service());
        setEmailPolicy(Wt::Auth::RegistrationModel::EmailMandatory);
    }

    void reset() {
        // FIXME http://redmine.emweb.be/issues/1421
        Wt::Auth::RegistrationModel::reset();
        setEmailPolicy(Wt::Auth::RegistrationModel::EmailMandatory);
    }
};

class RegistrationWidget : public Wt::Auth::RegistrationWidget {
public:
    RegistrationWidget(AuthWidget* authWidget = 0):
        Wt::Auth::RegistrationWidget(authWidget)
    { }

    void close() {
        delete parent(); // WContainerWidget
    }
};

Wt::WWidget* AuthWidget::createRegistrationView(const Wt::Auth::Identity& id) {
    Wt::WContainerWidget* result = new Wt::WContainerWidget();
    if (!model_) {
        model_ = createRegistrationModel();
    } else {
        model_->reset();
    }
    if (id.isValid()) {
        model_->registerIdentified(id);
    }
    RegistrationWidget* w = new RegistrationWidget(this);
    w->setModel(model_);
    result->addWidget(w);
    result->addWidget(new Wt::WText(tr("tc.user.Password_example")));
    result->addWidget(new Wt::WText(Wt::Wc::good_password()));
    return result;
}

Wt::Auth::RegistrationModel* AuthWidget::createRegistrationModel() {
    return new RegistrationModel(this);
}

}

