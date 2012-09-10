/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WLineEdit>
#include <Wt/WTextEdit>
#include <Wt/WCheckBox>
#include <Wt/WBreak>
#include <Wt/Wc/TimeDurationWidget.hpp>

#include "widgets/user/SettingsWidget.hpp"
#include "widgets/user/ClassificationWidget.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "utils/text_edit.hpp"

namespace thechess {

class SettingsWidget::SettingsWidgetImpl : public Wt::WContainerWidget {
public:
    SettingsWidgetImpl() {
        dbo::Transaction t(tApp->session());
        if (!tApp->user()) {
            return;
        }
        new Header(tr("tc.user.Settings"), this);
        print_password_changer();
        print_email_changer();
        print_avater_note();
        if (tApp->user()->has_permission(CLASSIFICATION_CHANGER)) {
            print_classification_changer();
        }
        print_settings_changer();
        print_filter_min_online();
        print_description_changer();
        if (!tApp->user()->vacation_until().isValid() ||
                tApp->user()->vacation_until() < now()) {
            print_vacation();
        }
        print_recalculation();
    }

private:
    ClassificationWidget* class_;
    Wt::WLineEdit* email_;
    Wt::WCheckBox* public_email_;
    Wt::WCheckBox* names_in_mymenu_;
    Wt::WTextEdit* description_;
    Wt::Wc::TimeDurationWidget* vacation_duration_;
    Wt::Wc::TimeDurationWidget* filter_min_online_;

    void print_password_changer() {
        new Wt::WBreak(this);
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr("Wt.Auth.updatepassword"), this);
        b->clicked().connect(tApp, &Application::update_password);
    }

    void print_email_changer() {
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.user.Email"), this);
        email_ = new Wt::WLineEdit(tApp->user()->email(), this);
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr("tc.common.Save"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::save_email);
    }

    void print_avater_note() {
        new Wt::WBreak(this);
        Wt::WAnchor* a = new Wt::WAnchor("http://gravatar.com", this);
        a->setText(tr("tc.user.Change_avatar"));
        a->setTarget(Wt::TargetNewWindow);
    }

    void print_classification_changer() {
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.user.classification"), this);
        class_ = new ClassificationWidget(NO_CLASSIFICATION,
                                          tApp->user()->classification(),
                                          SUPER_GRANDMASTER, this);
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::save_classification);
    }

    void print_settings_changer() {
        new Wt::WBreak(this);
        public_email_ = new Wt::WCheckBox(tr("tc.user.Public_email"), this);
        public_email_->setChecked(User::has_s(SWITCH_PUBLIC_EMAIL));
        new Wt::WBreak(this);
        names_in_mymenu_ = new Wt::WCheckBox(tr("tc.user.Names_in_menu"), this);
        names_in_mymenu_->setChecked(User::has_s(SWITCH_NAMES_IN_MYMENU));
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::save_settings);
    }

    void print_filter_min_online() {
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.user.Filter_min_online"), this);
        filter_min_online_ = new Wt::Wc::TimeDurationWidget(TD_NULL,
                tApp->user()->filter_min_online(), 100 * WEEK,
                this);
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::save_filter_min_online);
    }

    void print_description_changer() {
        new Wt::WBreak(this);
        description_ = new Wt::WTextEdit(tApp->user()->description(), this);
        patch_text_edit(description_);
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::save_description);
    }

    void print_vacation() {
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.user.Vacation_until").arg(""), this);
        vacation_duration_ = new Wt::Wc::TimeDurationWidget(DAY, 5 * DAY,
                50 * WEEK, this);
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::save_vacation);
    }

    void print_recalculation() {
        new Wt::WBreak(this);
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr("tc.user.Recalculation"), this);
        b->clicked().connect(this, &SettingsWidgetImpl::recalculation);
    }

    void save_classification() {
        dbo::Transaction t(tApp->session());
        if (!tApp->user()) {
            return;
        }
        tApp->user().reread();
        if (tApp->user()->has_permission(CLASSIFICATION_CHANGER)) {
            tApp->user().modify()->set_classification(class_->value());
        }
    }

    void save_email() {
        dbo::Transaction t(tApp->session());
        if (!tApp->user()) {
            return;
        }
        std::string email = email_->text().toUTF8();
        if (email.length() < 3 || email.find('@') == std::string::npos) {
            return;
        }
        delete sender();
        if (email != tApp->user()->email()) {
            tApp->user().modify()->set_email(email);
        }
    }

    void save_settings() {
        User::set_s(SWITCH_PUBLIC_EMAIL, public_email_->isChecked());
        bool names_in_mymenu = User::has_s(SWITCH_NAMES_IN_MYMENU);
        User::set_s(SWITCH_NAMES_IN_MYMENU, names_in_mymenu_->isChecked());
        if (names_in_mymenu != User::has_s(SWITCH_NAMES_IN_MYMENU)) {
            tApp->update_my_games();
        }
    }

    void save_filter_min_online() {
        dbo::Transaction t(tApp->session());
        tApp->user().reread();
        Td td = filter_min_online_->corrected_value();
        tApp->user().modify()->set_filter_min_online(td);
    }

    void save_description() {
        dbo::Transaction t(tApp->session());
        tApp->user().reread();
        Wt::WString description = patch_text_edit_text(description_->text());
        tApp->user().modify()->set_description(description);
    }

    void save_vacation() {
        dbo::Transaction t(tApp->session());
        tApp->user().reread();
        if (!tApp->user()->vacation_until().isValid() ||
                tApp->user()->vacation_until() < now()) {
            Td duration = vacation_duration_->corrected_value();
            Wt::WDateTime until = now() + duration;
            tApp->user().modify()->set_vacation_until(until);
            Games games = tApp->user()->games().resultList();
            BOOST_FOREACH (GamePtr g, games) {
                g.modify()->take_vacation_pause(duration);
            }
        }
    }

    void recalculation() {
        dbo::Transaction t(tApp->session());
        Users users = tApp->session().find<User>().resultList();
        BOOST_FOREACH (UserPtr user, users) {
            user.modify()->games_stat().reset();
            user.modify()->competitions_stat().reset();
        }
        Games games = tApp->session().find<Game>().resultList();
        BOOST_FOREACH (GamePtr game, games) {
            game.modify()->stat_change();
        }
        Competitions ccc = tApp->session().find<Competition>().resultList();
        BOOST_FOREACH (CompetitionPtr c, ccc) {
            c.modify()->stat_change();
        }
        GPs gps = tApp->session().find<GP>().resultList();
        BOOST_FOREACH (GPPtr gp, gps) {
            gp.modify()->set_games_size(gp->games().size());
        }
        CPs cps = tApp->session().find<CP>().resultList();
        BOOST_FOREACH (CPPtr cp, cps) {
            cp.modify()->set_competitions_size(cp->competitions().size());
        }
    }
};

SettingsWidget::SettingsWidget(Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new SettingsWidgetImpl();
    setImplementation(impl_);
}

}

