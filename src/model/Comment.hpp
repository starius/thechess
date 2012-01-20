/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMMENT_HPP_
#define THECHESS_MODEL_COMMENT_HPP_

#include "model/global.hpp"

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
        MODERATED = 30, /**< Suspicious message */
        SURROGATE = 40 /**< Created only to inherit from (no author or text) */
    };

    /** Constructor for Wt::Dbo internal usage */
    Comment();

    /** Create instance to be added to database */
    Comment(bool);

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

    /** Set the parent comment */
    void set_parent(const CommentPtr& parent) {
        parent_ = parent;
    }

    /** Get creation time */
    const Wt::WDateTime& created() const {
        return created_;
    }

    /** Get last edition (including creation) time */
    const Wt::WDateTime& edited() const {
        return edited_;
    }

    /** Wt::Dbo persist implementation */
    template<class Action>
    void persist(Action& a) {
        dbo::field(a, commentable_, "commentable");
        dbo::field(a, state_, "state");
        dbo::field(a, text_, "text");
        dbo::belongsTo(a, init_, "init");
        dbo::belongsTo(a, root_, "root");
        dbo::belongsTo(a, parent_, "parent");
        dbo::field(a, created_, "created");
        dbo::field(a, edited_, "edited");
    }

private:
    bool commentable_;
    State state_;
    Wt::WString text_;
    UserPtr init_;
    CommentPtr root_;
    CommentPtr parent_;
    Wt::WDateTime created_;
    Wt::WDateTime edited_;
};

}

#endif

