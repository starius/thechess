/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_TEAM_HPP_
#define THECHESS_MODEL_TEAM_HPP_

#include <map>

#include "model/model.hpp"
#include "model/record.hpp"

namespace thechess {

/** Model of team.

\ingroup model
*/
class Team : public dbo::Dbo<Team>, public Record {
public:
    /** Default constructor.
    Should be used only by Wt::Dbo itself.
    */
    Team();

    /** Create instance to be added to database */
    Team(bool);

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        Record::persist(a);
        dbo::hasMany(a, members_, dbo::ManyToMany, "team_members");
        dbo::hasMany(a, candidates_, dbo::ManyToMany, "team_candidates");
        dbo::hasMany(a, banned_, dbo::ManyToMany, "team_banned");
        dbo::belongsTo(a, comment_base_, "comment_base");
        dbo::field(a, removed_, "removed");
        dbo::hasMany(a, competitions_, dbo::ManyToMany, "teams_competitions");
        dbo::hasMany(a, won_competitions_, dbo::ManyToMany,
                     "winner_teams_competitions");
    }

    /** Team members */
    const Users& members() const {
        return members_;
    }

    /** Team members */
    Users& members() {
        return members_;
    }

    /** Team candidates */
    const Users& candidates() const {
        return candidates_;
    }

    /** Team candidates */
    Users& candidates() {
        return candidates_;
    }

    /** Team banned members */
    const Users& banned() const {
        return banned_;
    }

    /** Team banned members */
    Users& banned() {
        return banned_;
    }

    /** \name Methods of comments */
    /* @{ */

    /** Return if there is a comment base  */
    bool has_comment_base() const;

    /** Return comment base.
    Lazy created.
    */
    const CommentPtr& comment_base();

    /** Get comment base (may return 0) */
    const CommentPtr& comment_base() const {
        return comment_base_;
    }

    /* @} */

    /** Get the team is removed */
    bool removed() const {
        return removed_;
    }

    /** Set the team is removed */
    void set_removed(bool removed) {
        removed_ = removed;
    }

private:
    Users members_;
    Users candidates_;
    Users banned_;
    CommentPtr comment_base_;
    bool removed_;
    Competitions competitions_;
    Competitions won_competitions_;
};

/** Add new message to team chat.
Message is prefixed with [auto].
*/
void team_chat(const TeamPtr& team, const Wt::WString& message,
               const UserPtr& user = UserPtr());

/** Return if the user can create a team */
bool can_create_team(const UserPtr& user);

/** Try to create a team and return it on success */
TeamPtr create_team(const UserPtr& user);

/** Return if the user can remove the team */
bool can_remove_team(const UserPtr& user, const TeamPtr& team);

/** Try remove the team */
void remove_team(const UserPtr& user, const TeamPtr& team);

/** Return if the user can restore the team */
bool can_restore_team(const UserPtr& user, const TeamPtr& team);

/** Try restore the team */
void restore_team(const UserPtr& user, const TeamPtr& team);

/** Return if the user can edit the team */
bool can_edit_team(const UserPtr& user, const TeamPtr& team);

/** Return if the user can add himself to team's candidates */
bool can_join_team(const UserPtr& user, const TeamPtr& team);

/** Try add himself to team's candidates */
void join_team(const UserPtr& user, const TeamPtr& team);

/** Return if the user can approve the team candidate */
bool can_approve_team_candidate(const UserPtr& user, const TeamPtr& team,
                                const UserPtr& candidate);

/** Try approve the team candidate */
void approve_team_candidate(const UserPtr& user, const TeamPtr& team,
                            const UserPtr& candidate);

/** Return if the user can discard the team candidate */
bool can_discard_team_candidate(const UserPtr& user, const TeamPtr& team,
                                const UserPtr& candidate);

/** Try discard the team candidate */
void discard_team_candidate(const UserPtr& user, const TeamPtr& team,
                            const UserPtr& candidate);

/** Return if the user can remove user from team members list */
bool can_remove_team_members(const UserPtr& user, const TeamPtr& team,
                             const UserPtr& member);

/** Try remove user from team members list */
void remove_team_member(const UserPtr& user, const TeamPtr& team,
                        const UserPtr& candidate);

/** Return if the user can remove user from team ban list */
bool can_remove_team_banned(const UserPtr& user, const TeamPtr& team,
                            const UserPtr& banned);

/** Try remove user from team ban list */
void remove_team_banned(const UserPtr& user, const TeamPtr& team,
                        const UserPtr& banned);

/** Return if the user can list banned users of the team */
bool can_list_team_banned(const UserPtr& user, const TeamPtr& team);

/** Return if the user can set other user as team leader */
bool can_set_team_leader(const UserPtr& user, const TeamPtr& team,
                         const UserPtr& new_leader);

/** Try set other user as team leader */
void set_team_leader(const UserPtr& user, const TeamPtr& team,
                     const UserPtr& new_leader);

}

DBO_EXTERN_TEMPLATES(thechess::Team);

#endif

