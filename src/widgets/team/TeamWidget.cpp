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
#include "widgets/Header.hpp"

namespace thechess {

TeamWidget::TeamWidget(const TeamPtr& team):
    Notifiable(Object(TEAM_OBJECT, team.id())),
    team_(team) {
    dbo::Transaction t(tApp->session());
    reprint();
}

void TeamWidget::notify(EventPtr) {
    dbo::Transaction t(tApp->session());
    team_.reread();
    reprint();
}

void TeamWidget::reprint() {
    clear();
    print_title();
    print_members();
    print_candidates();
    print_manager();
}

void TeamWidget::print_title() {
    addWidget(new Header(tr("tc.team.Title")
                         .arg(team_.id()).arg(team_->name())));
    addWidget(new Wt::WText(tr("tc.common.Description")));
    addWidget(new Wt::WBreak);
    addWidget(new Wt::WText(team_->description()));
}

void TeamWidget::print_members() {
    addWidget(new Header(tr("tc.team.Members")));
    list_users(team_->members());
}

void TeamWidget::print_candidates() {
    addWidget(new Header(tr("tc.team.Candidates")));
    list_users(team_->candidates());
}

void TeamWidget::print_banned() {
    if (can_list_team_banned(tApp->user(), team_)) {
        addWidget(new Header(tr("tc.team.Banned")));
        list_users(team_->banned());
    }
}

void TeamWidget::print_manager() {
    if (can_remove_team(tApp->user(), team_)) {
        add_button(tr("tc.team.Remove_team"), remove_team, this);
    }
    if (can_restore_team(tApp->user(), team_)) {
        add_button(tr("tc.team.Restore_team"), restore_team, this);
    }
    if (can_join_team(tApp->user(), team_)) {
        add_button(tr("tc.common.Join"), join_team, this);
    }
    if (can_edit_team(tApp->user(), team_)) {
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.team.Change"), this);
        b->clicked().connect(this, &TeamWidget::show_edit);
    }
}

void TeamWidget::list_users(const Users& users) {
    Wt::WContainerWidget* list = new Wt::WContainerWidget(this);
    list->setList(true);
    UsersVector users_copy(users.begin(), users.end());
    BOOST_FOREACH (const UserPtr& user, users_copy) {
        Wt::WContainerWidget* item = new Wt::WContainerWidget(list);
        user_anchor(user, item);
        user_buttons(user, item);
    }
}

void TeamWidget::user_buttons(const UserPtr& user, Wt::WContainerWidget* item) {
    if (can_change_team_candidate(tApp->user(), team_, user)) {
        add_button(tr("tc.common.Confirm"), boost::bind(change_team_candidate,
                   _1, _2, user, true), item);
        add_button(tr("tc.common.Discard"), boost::bind(change_team_candidate,
                   _1, _2, user, false), item);
    } else if (can_change_team_members(tApp->user(), team_, user)) {
        add_button(tr("tc.common.Discard"), boost::bind(remove_team_member,
                   _1, _2, user), item);
    } else if (can_change_team_banned(tApp->user(), team_, user)) {
        add_button(tr("tc.common.Discard"), boost::bind(remove_team_banned,
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
    clear();
    addWidget(new TeamEdit(team_));
}

}

