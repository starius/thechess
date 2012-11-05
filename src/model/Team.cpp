/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/all.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::Team);

namespace thechess {

Team::Team()
{ }

Team::Team(bool):
    removed_(false)
{ }

bool Team::has_comment_base() const {
    return comment_base_;
}

const CommentPtr& Team::comment_base() {
    if (!comment_base_) {
        comment_base_ = session()->add(new Comment(true));
        comment_base_.modify()->set_type(Comment::CHAT_ROOT);
        comment_base_.flush();
    }
    return comment_base_;
}

void team_chat(const TeamPtr& team, const Wt::WString& text,
               const UserPtr& user) {
    CommentPtr base = team->comment_base();
    if (!base) {
        base = team.modify()->comment_base();
        t_emit_after(TEAM_OBJECT, team.id());
    }
    CommentPtr message = team.session()->add(new Comment(true));
    message.modify()->set_text("[auto] " + text);
    message.modify()->set_init(user);
    message.modify()->set_parent(base);
    message.modify()->set_root(base);
    message.modify()->set_type(Comment::CHAT_MESSAGE);
    message.flush();
    t_emit(COMMENT, base.id());
}

bool can_create_team(const UserPtr& user) {
    return user->has_permission(CREATE_TEAM);
}

bool can_edit_team(const UserPtr& user, const TeamPtr& team) {
    return user->has_permission(RECORDS_EDITOR) ||
           user == team->init();
}

bool can_join_team(const UserPtr& user, const TeamPtr& team) {
    return user->has_permission(JOIN_TEAM) &&
           !team->members().count(user) &&
           !team->candidates().count(user) &&
           !team->banned().count(user);
}

bool can_change_team_candidate(const UserPtr& user, const TeamPtr& team,
                               const UserPtr& candidate) {
    return (user->has_permission(TEAM_CHANGER) || team->init() == user) &&
           team->candidates().count(user);
}

bool can_change_team_members(const UserPtr& user, const TeamPtr& team,
                             const UserPtr& candidate) {
    return (user->has_permission(TEAM_CHANGER) || team->init() == user) &&
           team->candidates().count(user);
}

bool can_change_team_banned(const UserPtr& user, const TeamPtr& team,
                            const UserPtr& banned) {
    return (user->has_permission(TEAM_CHANGER) || team->init() == user) &&
           team->banned().count(user);
}

}

