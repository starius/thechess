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

#include "widgets/user/UserWidget.hpp"
#include "widgets/user/RatingChanges.hpp"
#include "widgets/game/GameCreateWidget.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "utils/time_intervals.hpp"
#include "notify.hpp"

namespace thechess {

class UserWidget::UserWidgetImpl : public Wt::WContainerWidget {
public:
    UserWidgetImpl(const UserPtr& user) :
        Wt::WContainerWidget(), user_(user) {
        dbo::Transaction t(tApp->session());
        new Header(user_->username(), this);
        if (user_->online()) {
            new Wt::WText(tr("tc.user.Online"), this);
        }
        if (user_->removed()) {
            new Wt::WText(tr("tc.user.Removed_message"), this);
        }
        new Wt::WBreak(this);
        new Wt::WText(user_->classification_str(), this);
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
        if (user->games_stat().all() > 0) {
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
                tApp->user()->has_permission(User::VIRTUALS_VIEWER)) {
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

    void inverse_removed() {
        dbo::Transaction t(tApp->session());
        user_.reread();
        user_->auth_info().reread();
        if (tApp->user() && tApp->user()->can_remove(user_)) {
            user_.modify()->set_removed(!user_->removed());
        }
        t.commit();
        tNot->emit(new Object(USER, user_.id()));
        tApp->path().open(tApp->internalPath());
    }
};

UserWidget::UserWidget(const UserPtr& user, Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new UserWidgetImpl(user);
    setImplementation(impl_);
}

}

