/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WBreak>

#include "widgets/user/SettingsWidget.hpp"
#include "widgets/user/ClassificationWidget.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "model/all.hpp"

namespace thechess {

class SettingsWidget::SettingsWidgetImpl : public Wt::WContainerWidget {
public:
    SettingsWidgetImpl() {
        Wt::WPushButton* b;
        dbo::Transaction t(tApp->session());
        if (!tApp->user()) {
            return;
        }
        new Header(tr("tc.user.Settings"), this);
        b = new Wt::WPushButton(tr("Wt.Auth.updatepassword"), this);
        b->clicked().connect(tApp, &Application::update_password);
        if (tApp->user()->has_permission(CLASSIFICATION_CHANGER)) {
            print_classification_changer();
        }
    }

private:
    ClassificationWidget* class_;

    void print_classification_changer() {
        new Wt::WBreak(this);
        class_ = new ClassificationWidget(NO_CLASSIFICATION,
                                          tApp->user()->classification(),
                                          SUPER_GRANDMASTER, this);
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::save_classification);
    }

    void save_classification() {
        dbo::Transaction t(tApp->session());
        if (!tApp->user()) {
            return;
        }
        if (tApp->user()->has_permission(CLASSIFICATION_CHANGER)) {
            tApp->user().modify()->set_classification(class_->value());
        }
    }
};

SettingsWidget::SettingsWidget(Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new SettingsWidgetImpl();
    setImplementation(impl_);
}

}

