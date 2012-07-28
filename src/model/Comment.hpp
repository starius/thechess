/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMMENT_HPP_
#define THECHESS_MODEL_COMMENT_HPP_

#include "model/model.hpp"

namespace thechess {

/** Dbo model of comment and/or a base for commenting for other models.
The comment may be \ref SURROGATE "surrogate" -- created
to hold all comments of a game or other model.

\ingroup model
*/
class Comment : public dbo::Dbo<Comment> {
public:
    /** The state of comment */
    enum State {
        OK = 10, /**< Ok, message is shown (default value) */
        DELETED = 20, /**< Message is bad (hidden) */
        DRAFT = 30, /**< Suspicious message (pre-moderation) */
        SURROGATE = 40 /**< Created only to inherit from (no author or text) */
    };

    /** Constructor for Wt::Dbo internal usage */
    Comment();

    /** Create instance to be added to database */
    Comment(bool);

    /** Get sorting index.
    The order of indices of a branch should correspond
    the order in the tree for viewing.
    This field is not significant, it could be recalculated
    using parent() field.
    */
    double index() const {
        return index_;
    }

    /** Set sorting index */
    void set_index(double index) {
        index_ = index;
    }

    /** Set auto value for sorting index using information about parents */
    void set_index();

    /** Get depth.
    For root depth is 0, for its children -- 1 and so on.
    This field is not significant, it could be recalculated
    using parent() field.
    */
    int depth() const {
        return depth_;
    }

    /** Get if the comment can be commented (used as a parent) */
    bool commentable() const {
        return commentable_;
    }

    /** Set if the comment can be commented (default value is true) */
    void set_commentable(bool commentable) {
        commentable_ = commentable;
    }

    /** Get state */
    State state() const {
        return state_;
    }

    /** Set state */
    void set_state(State state) {
        state_ = state;
    }

    /** Get text */
    const Wt::WString& text() const {
        return text_;
    }

    /** Set text.
    This method updates edited() value.
    */
    void set_text(const Wt::WString& text);

    /** Get the user created the comment */
    const UserPtr& init() const {
        return init_;
    }

    /** Set the user created the comment */
    void set_init(const UserPtr& init) {
        init_ = init;
    }

    /** Get the root comment.
    Root of root comment is the root itself (not null).
    */
    const CommentPtr& root() const {
        return root_;
    }

    /** Set the root comment */
    void set_root(const CommentPtr& root) {
        root_ = root;
    }

    /** Get the parent comment.
    If the parent is not set, this means the comment is a root.
    In this case its state may be SURROGATE.
    */
    const CommentPtr& parent() const {
        return parent_;
    }

    /** Set the parent comment.
    \param set_index If the index should be also set (using set_index()).
    \param set_root If the root should be also set.
    This method also sets depth().
    */
    void set_parent(const CommentPtr& parent, bool set_index = true,
                    bool set_root = true);

    /** Get creation time */
    const Wt::WDateTime& created() const {
        return created_;
    }

    /** Get last edition (including creation) time */
    const Wt::WDateTime& edited() const {
        return edited_;
    }

    /** Get (direct) children */
    const Comments& children() const {
        return children_;
    }

    /** Get entire family of comments (nonempty for root instances) */
    const Comments& family() const {
        return family_;
    }

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::field(a, index_, "show_index");
        dbo::field(a, depth_, "depth");
        dbo::field(a, commentable_, "commentable");
        dbo::field(a, state_, "state");
        dbo::field(a, text_, "text");
        dbo::belongsTo(a, init_, "init");
        dbo::belongsTo(a, root_, "root");
        dbo::belongsTo(a, parent_, "parent");
        dbo::field(a, created_, "created");
        dbo::field(a, edited_, "edited");
        dbo::hasMany(a, children_, dbo::ManyToOne, "parent");
        dbo::hasMany(a, family_, dbo::ManyToOne, "root");
    }

private:
    double index_;
    int depth_;
    bool commentable_;
    State state_;
    Wt::WString text_;
    UserPtr init_;
    CommentPtr root_;
    CommentPtr parent_;
    Wt::WDateTime created_;
    Wt::WDateTime edited_;

    Comments children_;
    Comments family_;
};

}

#endif

