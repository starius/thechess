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

private:
    Users members_;
    Users candidates_;
    Users banned_;
    CommentPtr comment_base_;
};

}

DBO_EXTERN_TEMPLATES(thechess::Team);

#endif

