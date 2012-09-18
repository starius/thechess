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

\ingroup model
*/
class Comment : public dbo::Dbo<Comment> {
public:
    /** The type of comment */
    enum Type {
        NO_TYPE = 0, /**< Invalid type. E.g., child_type(CHAT_MESSAGE) */
        CHAT_ROOT = 50, /**< Commentable by CHAT_MESSAGE; no author or text */
        CHAT_MESSAGE = 55, /**< Non-commentable; one line;
                                root=CHAT_ROOT or 0 if global chat */
        FORUM_TOPIC = 60, /**< Commentable by FORUM_POST; one-line */
        FORUM_POST = 70, /**< Commentable by FORUM_POST_TEXT; one-line;
                              root=FORUM_TOPIC */
        FORUM_POST_TEXT = 71, /**< Commentable by FORUM_COMMENT; multi-line */
        FORUM_COMMENT = 75, /**< Commentable by FORUM_COMMENT; multi-line;
                                 root=FORUM_POST_TEXT */
        PRIVATE_MESSAGE = 90, /** Non-commentable; one line; root=CHAT_ROOT */
        LOG_ENTRY = 100 /** Security log */
    };

    /** The state of comment */
    enum State {
        OK = 10, /**< Ok, message is shown (default value) */
        DELETED = 20, /**< Message is bad (hidden) */
        DRAFT = 30 /**< Suspicious message (pre-moderation) */
    };

    /** Constructor for Wt::Dbo internal usage */
    Comment();

    /** Create instance to be added to database */
    Comment(bool);

    /** Return type of children */
    static Type child_type(Type type);

    /** Return type of root */
    static Type root_type(Type type);

    /** Return the state of new comment, created by the user.
    If returned value is DELETED, the user can't create a comment.
    Returned value OK or DRAFT is a state of new comment
    (it is being set automatically by set_type(), set_init() or set_parent()).
    */
    static State state_of_new(const UserPtr& user, Type type,
                              const CommentPtr& parent = CommentPtr());

    /** Return if the user can write a comment of the type.
    This is short alias for state_of_new() != DELETED.
    */
    static bool can_create(const UserPtr& user, Type type,
                           const CommentPtr& parent = CommentPtr());

    /** Return if the user can edit the comment */
    bool can_edit(const UserPtr& user) const;

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

    /** Set auto value for depth */
    void set_depth();

    /** Get depth.
    For root depth is 0, for its children -- 1 and so on.
    This field is not significant, it could be recalculated
    using parent() field.
    */
    int depth() const {
        return depth_;
    }

    /** Get type */
    Type type() const {
        return type_;
    }

    /** Set type */
    void set_type(Type type);

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

    /** Get text or text that a comment was deleted.
    If viewer has permissions to view removed comments,
    a removed comment will be shown, but with message,
    that it was removed.
    */
    Wt::WString text_or_removed(const UserPtr& viewer) const;

    /** Set text.
    This method updates edited() value.
    */
    void set_text(const Wt::WString& text);

    /** Get the user created the comment */
    const UserPtr& init() const {
        return init_;
    }

    /** Set the user created the comment */
    void set_init(const UserPtr& init);

    /** Get the root comment */
    const CommentPtr& root() const {
        return root_;
    }

    /** Set the root comment.
    This method set index, if parent is set.
    */
    void set_root(const CommentPtr& root);

    /** Get the parent comment */
    const CommentPtr& parent() const {
        return parent_;
    }

    /** Set the parent comment.
    This method also sets depth() and set index, if root is set.
    */
    void set_parent(const CommentPtr& parent);

    /** Get creation time.
    For FORUM_POST this means the time of last comment.
    */
    const Wt::WDateTime& created() const {
        return created_;
    }

    /** Update creation time of post when a comment it added */
    void post_comment_added();

    /** Get last edition (including creation) time */
    const Wt::WDateTime& edited() const {
        return edited_;
    }

    /** Return IP address from which this comment was created */
    const std::string& ip() const {
        return ip_;
    }

    /** Get score of the comment (not used) */
    int score() const {
        return score_;
    }

    /** Set score of the comment */
    void set_score(int score) {
        score_ = score;
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
        dbo::field(a, type_, "type");
        dbo::field(a, state_, "state");
        dbo::field(a, text_, "text");
        dbo::belongsTo(a, init_, "init");
        dbo::belongsTo(a, root_, "root");
        dbo::belongsTo(a, parent_, "parent");
        dbo::field(a, created_, "created");
        dbo::field(a, edited_, "edited");
        dbo::field(a, ip_, "ip");
        dbo::field(a, score_, "score");
        dbo::hasMany(a, children_, dbo::ManyToOne, "parent");
        dbo::hasMany(a, family_, dbo::ManyToOne, "root");
    }

private:
    double index_;
    int depth_;
    Type type_;
    State state_;
    Wt::WString text_;
    UserPtr init_;
    CommentPtr root_;
    CommentPtr parent_;
    Wt::WDateTime created_;
    Wt::WDateTime edited_;
    std::string ip_;
    int score_;

    Comments children_;
    Comments family_;
};

}

DBO_EXTERN_TEMPLATES(thechess::Comment);

#endif

