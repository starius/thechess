/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>
#include <iostream>

#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WPanel>

#include "widgets/user/UserWidget.hpp"
#include "widgets/user/Gravatar.hpp"
#include "widgets/user/RatingChanges.hpp"
#include "widgets/user/RightsEdit.hpp"
#include "widgets/game/GameCreateWidget.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/user/awards.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "utils/time_intervals.hpp"
#include "notify.hpp"
#include "log.hpp"

namespace thechess {

class UserWidget::UserWidgetImpl : public Wt::WContainerWidget {
public:
    UserWidgetImpl(const UserPtr& user) :
        Wt::WContainerWidget(), user_(user) {
        dbo::Transaction t(tApp->session());
        user_.reread();
        new Header(user_->username(), this);
        new Wt::WBreak(this);
        new Gravatar(user_, this);
        if (tApp->user() && user_->has_setting(SWITCH_PUBLIC_EMAIL)) {
            new Wt::WBreak(this);
            new Wt::WText(user_->email(), this);
        }
        new Wt::WBreak(this);
        if (user_->online()) {
            new Wt::WText(tr("tc.user.Online"), this);
        } else {
            new Wt::WText(tr("tc.user.Last_enter")
                          .arg(user_->last_enter().toString()), this);
        }
        if (user_->removed()) {
            new Wt::WBreak(this);
            new Wt::WText(tr("tc.user.Removed_message"), this);
        }
        new Wt::WBreak(this);
        new Wt::WText(user_->classification_str(), this);
        if (user_->classification_confirmed()) {
            new Wt::WText(tr("tc.user.confirmed_by"), this);
            user_anchor(user_->classification_confirmer(), this);
        } else if (user_->classification() != NO_CLASSIFICATION) {
            new Wt::WText(tr("tc.user.not_confirmed"), this);
        }
        if (user_->classification() != NO_CLASSIFICATION &&
                user_->can_confirm_classification(tApp->user())) {
            Wt::WPushButton* b = new Wt::WPushButton(this);
            if (user_->classification_confirmed()) {
                b->setText(tr("tc.common.Discard"));
            } else {
                b->setText(tr("tc.common.Confirm"));
            }
            b->clicked().connect(this, &UserWidgetImpl::inverse_confirmed);
        }
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.user.Online_time")
                      .arg(td2str(user_->online_time())), this);
        new Wt::WBreak(this);
        if (tApp->user() && tApp->user() != user_ && !user_->removed()) {
            start_button_ = new Wt::WPushButton(
                Wt::WString::tr("tc.user.Start_game"), this);
            start_button_->clicked()
            .connect(this, &UserWidgetImpl::game_form);
        }
        if (0) { //user->games_stat().all() > 0) {
            // crash in Wt 3.2.1
            rating_button_ = new Wt::WPushButton(
                Wt::WString::tr("tc.user.Rating_changes"), this);
            rating_button_->clicked()
            .connect(this, &UserWidgetImpl::rating_changes);
            if (tApp->user() && tApp->user() != user_) {
                rating_and_me_button_ = new Wt::WPushButton(
                    Wt::WString::tr("tc.user.Rating_changes_and_me"), this);
                rating_and_me_button_->clicked()
                .connect(this, &UserWidgetImpl::rating_changes_and_me);
            }
        }
        if (user->can_change_right(tApp->user())) {
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.user.Edit_rights"), this);
            b->clicked().connect(this, &UserWidgetImpl::edit_rights);
        }
        if (tApp->user() && tApp->user()->can_remove(user)) {
            Wt::WPushButton* b = new Wt::WPushButton(this);
            b->clicked().connect(this, &UserWidgetImpl::inverse_removed);
            if (user->removed()) {
                b->setText(tr("tc.user.Undelete"));
            } else {
                b->setText(tr("tc.user.Delete"));
            }
        }
        new Wt::WBreak(this);
        tApp->path().user_view()->set_integer_value(user.id());
        Wt::WAnchor* a;
        if (tApp->user() &&
                tApp->user()->has_permission(VIRTUALS_VIEWER)) {
            a = new Wt::WAnchor(this);
            a->setLink(tApp->path().virtuals_of_user()->link());
            a->setText(tr("tc.user.Virtuals_list"));
            new Wt::WBreak(this);
        }
        a = new Wt::WAnchor(this);
        a->setLink(tApp->path().games_of_user()->link());
        a->setText(tr("tc.game.List"));
        new Wt::WBreak(this);
        a = new Wt::WAnchor(this);
        a->setLink(tApp->path().competitions_of_user()->link());
        a->setText(tr("tc.competition.List"));
        new Wt::WBreak(this);
        a = new Wt::WAnchor(this);
        a->setLink(tApp->path().user_comments()->link());
        a->setText(tr("tc.forum.all_comments"));
        new Wt::WBreak(this);
        print_rights();
        print_game_stat();
        print_competition_stat();
    }

private:
    UserPtr user_;
    Wt::WPushButton* start_button_;
    Wt::WPushButton* rating_button_;
    Wt::WPushButton* rating_and_me_button_;

    void game_form() {
        start_button_->hide();
        new GameCreateWidget(user_, this);
    }

    void rating_changes() {
        rating_button_->hide();
        new RatingChanges(user_, this);
    }

    void rating_changes_and_me() {
        rating_and_me_button_->hide();
        RatingChanges* rc = new RatingChanges(user_, this);
        rc->add_user(tApp->user());
    }

    void edit_rights() {
        delete sender();
        new RightsEdit(user_, this);
    }

    void inverse_removed() {
        dbo::Transaction t(tApp->session());
        user_.reread();
        user_->auth_info().reread();
        if (tApp->user() && tApp->user()->can_remove(user_)) {
            user_.modify()->set_removed(!user_->removed());
        }
        if (user_->removed()) {
            admin_log("Remove user " + user_a(user_.id()));
        } else {
            admin_log("Unremove user " + user_a(user_.id()));
        }
        t.commit();
        tNot->emit(new Object(USER, user_.id()));
        tApp->path().open(tApp->internalPath());
    }

    void inverse_confirmed() {
        dbo::Transaction t(tApp->session());
        if (user_->classification_confirmed()) {
            user_.modify()->discard_classification(tApp->user());
            admin_log("Discard classification of " + user_a(user_.id()));
        } else {
            user_.modify()->confirm_classification(tApp->user());
            admin_log("Confirm classification of " + user_a(user_.id()));
        }
        t.commit();
        tApp->path().open(tApp->internalPath());
    }

    void print_right(User::Rights right, Wt::WContainerWidget* p) {
        if (user_->has_permission(right)) {
            Wt::WContainerWidget* item = new Wt::WContainerWidget(p);
            item->addWidget(new Wt::WText(tr(User::right_to_str(right))));
        }
    }

    void print_rights() {
        if (user_->rights() == NONE) {
            return;
        }
        Wt::WPanel* panel = new Wt::WPanel(this);
        panel->setCollapsible(true);
        panel->collapse();
        panel->setTitle(tr("tc.user.List_rights"));
        panel->setWidth(300);
        Wt::WContainerWidget* p = new Wt::WContainerWidget();
        p->setList(true);
        panel->setCentralWidget(p);
        User::for_each_right(boost::bind(&UserWidgetImpl::print_right,
                                         this, _1, p));
    }

    void print_game_stat() {
        addWidget(awards_widget(user_));
        const EloPlayer& stat = user_->games_stat();
        new Wt::WText(tr("tc.user.Games_stat"), this);
        new Wt::WText(tr("tc.common.Stat").arg(stat.all()).arg(stat.wins())
                      .arg(stat.draws()).arg(stat.fails()).arg(stat.elo()),
                      this);
        if (tApp->user() && tApp->user() != user_) {
            EloPlayer my_stat = tApp->user()->games_stat();
            EloPlayer my_orig = my_stat;
            EloPlayer his_stat = user_->games_stat();
            my_stat.win(&his_stat);
            int if_win = my_stat.elo() - my_orig.elo();
            my_stat = tApp->user()->games_stat();
            his_stat = user_->games_stat();
            his_stat.win(&my_stat);
            int if_fail = my_stat.elo() - my_orig.elo();
            my_stat = tApp->user()->games_stat();
            his_stat = user_->games_stat();
            his_stat.draw(&my_stat);
            int if_draw = my_stat.elo() - my_orig.elo();
            new Wt::WText(tr("tc.user.Games_stat_changes")
                          .arg(if_win).arg(if_fail).arg(if_draw), this);
        }
    }

    void print_competition_stat() {
        const EloPlayer& stat = user_->competitions_stat();
        new Wt::WText(tr("tc.user.Competitions_stat"), this);
        new Wt::WText(tr("tc.common.Stat").arg(stat.all()).arg(stat.wins())
                      .arg(stat.draws()).arg(stat.fails()).arg(stat.elo()),
                      this);
    }
};

UserWidget::UserWidget(const UserPtr& user, Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new UserWidgetImpl(user);
    setImplementation(impl_);
}

}

