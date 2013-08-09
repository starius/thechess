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
#include <Wt/WLineEdit>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WText>
#include <Wt/WTemplate>
#include <Wt/WPanel>
#include <Wt/Wc/util.hpp>

#include "widgets/user/UserWidget.hpp"
#include "widgets/user/Gravatar.hpp"
#include "widgets/user/RatingChanges.hpp"
#include "widgets/user/RightsEdit.hpp"
#include "widgets/user/ClassificationWidget.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/user/awards.hpp"
#include "widgets/comment/CommentList.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "utils/time_intervals.hpp"
#include "log.hpp"
#include "config.hpp"

namespace thechess {

class UserWidget::UserWidgetImpl : public Wt::WContainerWidget,
    public Notifiable {
public:
    UserWidgetImpl(const UserPtr& user) :
        Wt::WContainerWidget(),
        Notifiable(Object(USER, user.id())),
        user_(user) {
        reprint();
    }

    void notify(EventPtr) {
        reprint();
    }

private:
    UserPtr user_;
    ClassificationWidget* class_;
    Wt::WPushButton* rating_button_;
    Wt::WPushButton* rating_and_me_button_;
    Wt::WTextEdit* description_;

    void reprint() {
        UserPtr user = user_;
        clear();
        Wt::WText* tmp;
        dbo::Transaction t(tApp->session());
        user_.reread();
        tApp->user().reread();
        tApp->path().user_view()->set_integer_value(user.id());
        new Header(user_->username(), this);
        if (user_->has_permission(SUPER_RIGHTS_CHANGER)) {
            new Wt::WText(tr("tc.user.Admin"), this);
            new Wt::WBreak(this);
            new Wt::WBreak(this);
        } else if (user_->admin_rights()) {
            new Wt::WText(tr("tc.user.Moder"), this);
            new Wt::WBreak(this);
            new Wt::WBreak(this);
        }
        print_description();
        Gravatar* gravatar = new Gravatar(user_, this);
        gravatar->set_size(250);
        if (tApp->user() && user_->has_setting(SWITCH_PUBLIC_EMAIL)) {
            new Wt::WBreak(this);
            new Wt::WText(user_->email(), this);
        }
        new Wt::WBreak(this);
        if (user_->online()) {
            new Wt::WText(tr("tc.user.Online"), this);
        } else {
            tmp = new Wt::WText(tr("tc.user.Last_online")
                                .arg(time2str(user_->last_online())), this);
            tmp->setStyleClass("thechess-datetime");
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
        if (tApp->user() &&
                tApp->user()->has_permission(CLASSIFICATION_CONFIRMER)) {
            class_ = new ClassificationWidget(NO_CLASSIFICATION,
                                              user->classification(),
                                              SUPER_GRANDMASTER, this);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.common.Save"), this);
            b->clicked().connect(this, &UserWidgetImpl::set_classification);
        }
        new Wt::WBreak(this);
        tmp = new Wt::WText(tr("tc.user.Online_time")
                            .arg(td2str(user_->online_time())), this);
        tmp->setStyleClass("thechess-datetime");
        new Wt::WBreak(this);
        tmp = new Wt::WText(tr("tc.user.Registration_date")
                            .arg(time2str(user_->registration_date())), this);
        tmp->setStyleClass("thechess-datetime");
        if (user_->vacation_until().isValid() &&
                user_->vacation_until() > now()) {
            new Wt::WBreak(this);
            tmp = new Wt::WText(tr("tc.user.Vacation_until")
                                .arg(time2str(user_->vacation_until())), this);
            tmp->setStyleClass("thechess-datetime");
            Wt::WPushButton* b;
            if (tApp->user() && tApp->user()->has_permission(TIME_WIZARD)) {
                b = new Wt::WPushButton(tr("tc.common.Discard"), this);
                b->clicked().connect(this, &UserWidgetImpl::discard_vacation);
            }
        }
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.user.ID").arg(user_.id()), this);
        new Wt::WBreak(this);
        if (tApp->user() && tApp->user() != user_ && !user_->removed() &&
                !User::is_blocked(tApp->user(), user_) &&
                tApp->user()->has_permission(GAME_CREATOR)) {
            Wt::WAnchor* start_game = new Wt::WAnchor(this);
            start_game->setText(tr("tc.user.Start_game"));
            start_game->setLink(tApp->path().user_challenge()->link());
            start_game->addStyleClass("thechess-main-button");
            new Wt::WBreak(this);
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
        if (tApp->user() && tApp->user()->can_remove(user) &&
                !user->vacation_until().isValid()) {
            Wt::WPushButton* b = new Wt::WPushButton(this);
            b->clicked().connect(this, &UserWidgetImpl::inverse_removed);
            if (user->removed()) {
                b->setText(tr("tc.user.Undelete"));
            } else {
                b->setText(tr("tc.user.Delete"));
            }
            b = new Wt::WPushButton(tr("tc.common.Kick"), this);
            b->clicked().connect(this, &UserWidgetImpl::kick);
        }
        new Wt::WBreak(this);
        Wt::WAnchor* a;
        if (tApp->user() &&
                tApp->user()->has_permission(VIRTUALS_VIEWER)) {
            a = new Wt::WAnchor(this);
            a->setLink(tApp->path().virtuals_of_user()->link());
            a->setText(tr("tc.user.Virtuals_list"));
            new Wt::WBreak(this);
            a = new Wt::WAnchor(this);
            a->setLink(tApp->path().ip_of_user()->link());
            a->setText(tr("tc.user.IpList"));
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
        print_block();
        print_edit_description();
    }

    void discard_vacation() {
        dbo::Transaction t(tApp->session());
        if (tApp->user() && tApp->user()->has_permission(TIME_WIZARD)) {
            user_.reread();
            user_.modify()->set_vacation_until(Wt::WDateTime());
            admin_log("Discard vacation of " + user_a(user_.id()));
        }
        t.commit();
        t_emit(USER, user_.id());
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
        t_emit_after(USER, user_.id());
        t_emit("kick-" + TO_S(user_.id()));
    }

    void kick() {
        dbo::Transaction t(tApp->session());
        Wt::WDateTime d = now() + config::defaults::KICK_VACATION;
        user_.modify()->set_vacation_until(d);
        admin_log("Kick user " + user_a(user_.id()));
        t.commit();
        t_task(USER, user_.id());
        t_emit("kick-" + TO_S(user_.id()));
    }

    void set_classification() {
        dbo::Transaction t(tApp->session());
        if (!tApp->user() ||
                !tApp->user()->has_permission(CLASSIFICATION_CONFIRMER)) {
            return;
        }
        user_.reread();
        user_.modify()->set_classification(class_->value());
        admin_log("Change classification of " + user_a(user_.id()));
        t.commit();
        t_emit(USER, user_.id());
    }

    void inverse_confirmed() {
        dbo::Transaction t(tApp->session());
        user_.reread();
        if (user_->classification_confirmed()) {
            user_.modify()->discard_classification(tApp->user());
            admin_log("Discard classification of " + user_a(user_.id()));
        } else {
            user_.modify()->confirm_classification(tApp->user());
            admin_log("Confirm classification of " + user_a(user_.id()));
        }
        t.commit();
        t_emit(USER, user_.id());
    }

    void print_right(User::Rights right, Wt::WContainerWidget* p) {
        if (user_->has_permission(right)) {
            Wt::WContainerWidget* item = new Wt::WContainerWidget(p);
            item->addWidget(new Wt::WText(tr(User::right_to_str(right))));
        }
    }

    void print_admin_right(AdminRights right, Wt::WContainerWidget* p) {
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
        User::for_each_admin(boost::bind(&UserWidgetImpl::print_admin_right,
                                         this, _1, p));
    }

    void print_game_stat() {
        addWidget(awards_widget(user_));
        const EloPlayer& stat = user_->games_stat();
        new Wt::WText(tr("tc.user.Games_stat"), this);
        print_stat(stat, true);
        if (tApp->user() && tApp->user() != user_) {
            print_our_stat();
            print_expectations();
        }
    }

    void print_our_stat() {
        typedef dbo::Query<int> Q;
        Q base_q = tApp->session().query<int>(
                       "select count(1) from thechess_game where "
                       "winner_game_id = ? and (white_id = ? or black_id = ?)");
        int my = Q(base_q).bind(tApp->user()).bind(user_).bind(user_);
        int his = Q(base_q).bind(user_).bind(tApp->user()).bind(tApp->user());
        Q draws_q = tApp->session().query<int>(
                        "select count(1) from thechess_game "
                        "where state >= ? and state <= ? and "
                        "(white_id = ? or white_id = ?) and "
                        "(black_id = ? or black_id = ?)");
        int draws = Q(draws_q).bind(Game::MIN_DRAW).bind(Game::MAX_DRAW)
                    .bind(tApp->user()).bind(user_)
                    .bind(tApp->user()).bind(user_);
        int all = my + his + draws;
        if (all > 0) {
            new Wt::WBreak(this);
            new Wt::WText(tr("tc.user.Our_games_stat")
                          .arg(all).arg(my).arg(his).arg(draws)
                          .arg(user_->safe_username()), this);
        }
    }

    void print_expectations() {
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
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.user.Games_stat_changes")
                      .arg(if_win).arg(if_fail).arg(if_draw), this);
    }

    void print_competition_stat() {
        const EloPlayer& stat = user_->competitions_stat();
        new Wt::WText(tr("tc.user.Competitions_stat"), this);
        print_stat(stat, false);
    }

    void print_stat(const EloPlayer& stat, bool draws_fails) {
        Wt::WTemplate* count;
        if (draws_fails) {
            count = new Wt::WTemplate(tr("tc.common.Stat_draws_template"));
        } else {
            count = new Wt::WTemplate(tr("tc.common.Stat_template"));
        }
        addWidget(count);
        count->addFunction("tr", &Wt::WTemplate::Functions::tr);
        count->bindInt("all", stat.all());
        count->bindInt("wins", stat.wins());
        if (draws_fails) {
            count->bindInt("draws", stat.draws());
            count->bindInt("fails", stat.fails());
        }
        count->bindInt("elo", stat.elo());
    }

    void print_description() {
        if (!user_->description().empty()) {
            new Wt::WText(tr("tc.common.Description") + ": ", this);
            new Wt::WText(user_->safe_description(), this);
            new Wt::WBreak(this);
            new Wt::WBreak(this);
        }
    }

    void print_block() {
        if (tApp->user() && tApp->user() != user_) {
            new Wt::WBreak(this);
            Wt::WPushButton* b = new Wt::WPushButton(this);
            b->clicked().connect(this, &UserWidgetImpl::inverse_blocked);
            if (User::is_blocked(user_, tApp->user())) {
                b->setText(tr("tc.user.Unblock"));
            } else {
                b->setText(tr("tc.user.Block"));
                b->addStyleClass("thechess-dangerous");
            }
        }
        int i_blocked_them_size = user_->i_blocked_them().size();
        if (i_blocked_them_size) {
            new Wt::WBreak(this);
            new Wt::WText(tr("tc.user.I_blocked_them")
                          .arg(i_blocked_them_size), this);
        }
        int they_blocked_me_size = user_->they_blocked_me().size();
        if (they_blocked_me_size) {
            new Wt::WBreak(this);
            new Wt::WText(tr("tc.user.They_blocked_me")
                          .arg(they_blocked_me_size), this);
        }
    }

    void print_edit_description() {
        if (tApp->user() && tApp->user()->has_permission(RECORDS_EDITOR)) {
            new Wt::WBreak(this);
            Wt::WText* d = new Wt::WText(tr("tc.common.Description"), this);
            d->decorationStyle().font().setWeight(Wt::WFont::Bolder);
            new Wt::WBreak(this);
            description_ = new Wt::WTextEdit(user_->safe_description(), this);
            patch_text_edit(description_);
            Wt::WPushButton* b;
            b = new Wt::WPushButton(tr("tc.common.Save"), this);
            b->clicked().connect(this, &UserWidgetImpl::set_description);
        }
    }

    void inverse_blocked() {
        dbo::Transaction t(tApp->session());
        tApp->user().reread();
        if (tApp->user()) {
            if (User::is_blocked(user_, tApp->user())) {
                tApp->user().modify()->remove_from_my_filter(user_);
            } else {
                tApp->user().modify()->add_to_my_filter(user_);
            }
        }
        t.commit();
        t_emit(USER, user_.id());
    }

    void set_description() {
        dbo::Transaction t(tApp->session());
        if (tApp->user() && tApp->user()->has_permission(RECORDS_EDITOR)) {
            user_.reread();
            user_.modify()->set_description(description_->text());
            admin_log("Change description of " + user_a(user_.id()));
            t.commit();
            t_emit(USER, user_.id());
        }
    }
};

UserWidget::UserWidget(const UserPtr& user, Wt::WContainerWidget* parent) :
    WContainerWidget(parent) {
    addWidget(new UserWidgetImpl(user));
    addWidget(new CommentList(Comment::PRIVATE_MESSAGE,
                              /* root */ CommentPtr(),
                              user));
}

}

