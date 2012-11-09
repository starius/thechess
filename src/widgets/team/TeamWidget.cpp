/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <boost/foreach.hpp>

#include <Wt/WBreak>
#include <Wt/WText>

#include "widgets/team/TeamWidget.hpp"
#include "widgets/team/TeamEdit.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/comment/CommentList.hpp"
#include "widgets/Header.hpp"

namespace thechess {

TeamWidget::TeamWidget(const TeamPtr& team):
    Notifiable(Object(TEAM_OBJECT, team.id())),
    team_(team) {
    dbo::Transaction t(tApp->session());
    main_ = new Wt::WContainerWidget(this);
    print_chat();
    reprint();
}

void TeamWidget::notify(EventPtr) {
    dbo::Transaction t(tApp->session());
    team_.reread();
    reprint();
}

void TeamWidget::reprint() {
    main_->clear();
    print_title();
    print_members();
    print_candidates();
    print_banned();
    print_manager();
}

void TeamWidget::print_title() {
    main_->addWidget(new Header(tr("tc.team.Title")
                                .arg(team_.id()).arg(team_->name())));
    main_->addWidget(new Wt::WText(tr("tc.common.Description")));
    main_->addWidget(new Wt::WBreak);
    main_->addWidget(new Wt::WText(team_->description()));
    main_->addWidget(new Wt::WBreak);
    main_->addWidget(new Wt::WText(tr("tc.team.Leader")));
    main_->addWidget(new Wt::WText(" "));
    user_anchor(team_->init(), main_);
    main_->addWidget(new Wt::WBreak);
    Wt::WText* date = new Wt::WText(time2str(team_->created()));
    date->addStyleClass("thechess-datetime");
    main_->addWidget(date);
    main_->addWidget(new Wt::WBreak);
}

void TeamWidget::print_members() {
    list_users(team_->members(), tr("tc.team.Members"));
}

void TeamWidget::print_candidates() {
    list_users(team_->candidates(), tr("tc.team.Candidates"));
}

void TeamWidget::print_banned() {
    if (can_list_team_banned(tApp->user(), team_)) {
        list_users(team_->banned(), tr("tc.team.Banned"));
    }
}

void TeamWidget::print_manager() {
    if (can_remove_team(tApp->user(), team_)) {
        add_button(tr("tc.team.Remove_team"), remove_team, main_);
    }
    if (can_restore_team(tApp->user(), team_)) {
        add_button(tr("tc.team.Restore_team"), restore_team, main_);
    }
    if (can_join_team(tApp->user(), team_)) {
        add_button(tr("tc.common.Join"), join_team, main_);
    }
    if (can_edit_team(tApp->user(), team_)) {
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.team.Change"), main_);
        b->clicked().connect(this, &TeamWidget::show_edit);
    }
}

void TeamWidget::print_chat() {
    if (team_->has_comment_base()) {
        addWidget(new Wt::WBreak);
        addWidget(new Wt::WBreak);
        addWidget(new CommentList(Comment::CHAT_MESSAGE,
                                  team_->comment_base()));
    }
}

void TeamWidget::list_users(const Users& users, const Wt::WString& header) {
    UsersVector users_copy(users.begin(), users.end());
    if (users_copy.empty()) {
        return;
    }
    main_->addWidget(new Wt::WBreak);
    main_->addWidget(new Header(header));
    Wt::WContainerWidget* list = new Wt::WContainerWidget(main_);
    list->setList(true);
    BOOST_FOREACH (const UserPtr& user, users_copy) {
        Wt::WContainerWidget* item = new Wt::WContainerWidget(list);
        user_anchor(user, item);
        user_buttons(user, item);
    }
}

void TeamWidget::user_buttons(const UserPtr& user, Wt::WContainerWidget* item) {
    if (can_approve_team_candidate(tApp->user(), team_, user)) {
        add_button(tr("tc.common.Confirm"), boost::bind(approve_team_candidate,
                   _1, _2, user), item);
    }
    if (can_discard_team_candidate(tApp->user(), team_, user)) {
        add_button(tr("tc.common.Discard"), boost::bind(discard_team_candidate,
                   _1, _2, user), item);
    }
    if (can_remove_team_members(tApp->user(), team_, user)) {
        add_button(tr("tc.common.Discard"), boost::bind(remove_team_member,
                   _1, _2, user), item);
    }
    if (can_remove_team_banned(tApp->user(), team_, user)) {
        add_button(tr("tc.common.Discard"), boost::bind(remove_team_banned,
                   _1, _2, user), item);
    }
    if (can_set_team_leader(tApp->user(), team_, user)) {
        add_button(tr("tc.team.SetLeader"), boost::bind(set_team_leader,
                   _1, _2, user), item);
    }
}

void TeamWidget::add_button(const Wt::WString& button,
                            const TeamWidget::UserAction& user_action,
                            Wt::WContainerWidget* item) {
    Wt::WPushButton* b = new Wt::WPushButton(button, item);
    b->clicked().connect(boost::bind(&TeamWidget::apply_action, this,
                                     boost::protect(user_action)));
}

void TeamWidget::apply_action(const TeamWidget::UserAction& user_action) {
    dbo::Transaction t(tApp->session());
    team_.reread();
    user_action(tApp->user(), team_);
    t.commit();
    t_emit(TEAM_OBJECT, team_.id());
}

void TeamWidget::show_edit() {
    dbo::Transaction t(tApp->session());
    main_->clear();
    main_->addWidget(new TeamEdit(team_));
}

}

