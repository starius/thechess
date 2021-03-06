/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/all.hpp"
#include "log.hpp"

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
    return user && user->has_permission(CREATE_TEAM);
}

TeamPtr create_team(const UserPtr& user) {
    TeamPtr team;
    if (can_create_team(user)) {
        team = user.session()->add(new Team(true));
        team.modify()->set_init(user);
        team.modify()->members().insert(user);
        team.flush();
        team_chat(team, "team created", user);
    }
    return team;
}

static bool is_team_manager(const UserPtr& user, const TeamPtr& team) {
    return (user && user->has_permission(TEAM_CHANGER)) ||
           team->init() == user;
}

bool can_remove_team(const UserPtr& user, const TeamPtr& team) {
    return is_team_manager(user, team) && !team->removed();
}

void remove_team(const UserPtr& user, const TeamPtr& team) {
    if (can_remove_team(user, team)) {
        team.modify()->set_removed(true);
        team_chat(team, "team removed", user);
    }
}

bool can_restore_team(const UserPtr& user, const TeamPtr& team) {
    return user && user->has_permission(TEAM_CHANGER) && team->removed();
}

void restore_team(const UserPtr& user, const TeamPtr& team) {
    if (can_restore_team(user, team)) {
        team.modify()->set_removed(false);
        team_chat(team, "team restored", user);
    }
}

bool can_edit_team(const UserPtr& user, const TeamPtr& team) {
    return (user && user->has_permission(RECORDS_EDITOR)) ||
           user == team->init();
}

bool can_join_team(const UserPtr& user, const TeamPtr& team) {
    return user && user->has_permission(JOIN_TEAM) &&
           !team->removed() &&
           !team->members().count(user) &&
           !team->candidates().count(user) &&
           !team->banned().count(user);
}

void join_team(const UserPtr& user, const TeamPtr& team) {
    if (can_join_team(user, team)) {
        team.modify()->candidates().insert(user);
        team_chat(team, "join candidates", user);
    }
}

bool can_approve_team_candidate(const UserPtr& user, const TeamPtr& team,
                                const UserPtr& candidate) {
    return is_team_manager(user, team) && team->candidates().count(candidate);
}

void approve_team_candidate(const UserPtr& user, const TeamPtr& team,
                            const UserPtr& candidate) {
    if (can_approve_team_candidate(user, team, candidate)) {
        team.modify()->candidates().erase(candidate);
        team.modify()->members().insert(candidate);
        team_chat(team, "add member " + user_a(candidate.id()), user);
    }
}

bool can_discard_team_candidate(const UserPtr& user, const TeamPtr& team,
                                const UserPtr& candidate) {
    return team->candidates().count(candidate) &&
           (is_team_manager(user, team) || user == candidate);
}

void discard_team_candidate(const UserPtr& user, const TeamPtr& team,
                            const UserPtr& candidate) {
    if (can_discard_team_candidate(user, team, candidate)) {
        team.modify()->candidates().erase(candidate);
        if (candidate == user) {
            team_chat(team, "leave candidates", user);
        } else {
            team.modify()->banned().insert(candidate);
            team_chat(team, "ban member " + user_a(candidate.id()), user);
        }
    }
}

bool can_remove_team_members(const UserPtr& user, const TeamPtr& team,
                             const UserPtr& member) {
    return team->members().count(member) &&
           (user == member ||
            (member != team->init() && is_team_manager(user, team)));
}

void remove_team_member(const UserPtr& user, const TeamPtr& team,
                        const UserPtr& member) {
    if (can_remove_team_members(user, team, member)) {
        team.modify()->members().erase(member);
        team_chat(team, "remove member " + user_a(member.id()), user);
    }
}

bool can_remove_team_banned(const UserPtr& user, const TeamPtr& team,
                            const UserPtr& banned) {
    return is_team_manager(user, team) && team->banned().count(banned);
}

void remove_team_banned(const UserPtr& user, const TeamPtr& team,
                        const UserPtr& banned) {
    if (can_remove_team_banned(user, team, banned)) {
        team.modify()->banned().erase(banned);
        team_chat(team, "unban " + user_a(banned.id()), user);
    }
}

bool can_list_team_banned(const UserPtr& user, const TeamPtr& team) {
    return is_team_manager(user, team);
}

bool can_set_team_leader(const UserPtr& user, const TeamPtr& team,
                         const UserPtr& new_leader) {
    return user && team->init() == user && new_leader && user != new_leader &&
           team->members().count(new_leader);
}

void set_team_leader(const UserPtr& user, const TeamPtr& team,
                     const UserPtr& new_leader) {
    if (can_set_team_leader(user, team, new_leader)) {
        team.modify()->set_init(new_leader);
        team_chat(team, "new team leader " + user_a(new_leader.id()), user);
    }
}

}

