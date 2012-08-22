/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "model/all.hpp"

DBO_INSTANTIATE_TEMPLATES(thechess::Comment);

namespace thechess {

const double COMMENT_STEP = 1;
const double COMMENT_GAP = 0.000001;

Comment::Comment()
{ }

Comment::Comment(bool):
    index_(0),
    depth_(0),
    type_(NO_TYPE),
    state_(OK),
    created_(now()),
    edited_(created_)
{ }

Comment::Type Comment::child_type(Comment::Type type) {
    if (type == CHAT_ROOT) {
        return CHAT_MESSAGE;
    } else if (type == FORUM_TOPIC) {
        return FORUM_POST;
    } else if (type == FORUM_POST) {
        return FORUM_POST_TEXT;
    } else if (type == FORUM_POST_TEXT) {
        return FORUM_COMMENT;
    } else if (type == FORUM_POST_TEXT || type == FORUM_COMMENT) {
        return FORUM_COMMENT;
    }
    return NO_TYPE;
}

Comment::Type Comment::root_type(Comment::Type type) {
    if (type == CHAT_MESSAGE) {
        return CHAT_ROOT;
    } else if (type == FORUM_POST) {
        return FORUM_TOPIC;
    } else if (type == FORUM_COMMENT) {
        return FORUM_POST_TEXT;
    }
    return NO_TYPE;
}

Comment::State Comment::state_of_new(const UserPtr& user, Type type,
                                     const CommentPtr& parent) {
    if (user) {
        if (type == CHAT_MESSAGE && !user->has_permission(User::CHAT_WRITER)) {
            return DELETED;
        }
        if (type == FORUM_TOPIC &&
                !user->has_permission(User::COMMENTS_REMOVER)) {
            return DELETED;
        }
        if (type == FORUM_POST &&
                !user->has_permission(User::FORUM_POST_CREATOR)) {
            return DELETED;
        }
        if (type == FORUM_COMMENT &&
                !user->has_permission(User::FORUM_COMMENT_CREATOR)) {
            return DELETED;
        }
        return OK;
    } else {
        return DRAFT;
    }
}

bool Comment::can_create(const UserPtr& user, Type type,
                         const CommentPtr& parent) {
    return state_of_new(user, type, parent) != DELETED;
}

bool Comment::can_edit(const UserPtr& user) const {
    if (type() != FORUM_POST && type() != FORUM_POST_TEXT) {
        return false;
    }
    if (!user || init() != user) {
        return false;
    }
    return can_create(user, type(), parent());
}

//FIXME *(collection.begin()) --> collection.front()
// FIXME 2 if Query family_desc = root_->family().find(), orderBy() fails
void Comment::set_index() {
    typedef dbo::Query<CommentPtr> Query;
    Query family_desc = session()->find<Comment>();
    CommentPtr r = root() ? : parent_->root() ? : parent_;
    family_desc.where("root_id = ?").bind(r);
    family_desc.where("id <> ?").bind(id()); // except me
    family_desc.orderBy("show_index desc");
    CommentPtr next;
    CommentPtr ancestor = parent_;
    while (ancestor && ancestor->parent() && ancestor != r) {
        Comments uncles = ancestor->parent()->children();
        Query uncles_asc = uncles.find().orderBy("show_index");
        double min = ancestor->index() + COMMENT_GAP;
        Comments older_uncles = uncles_asc.where("show_index > ?").bind(min);
        if (older_uncles.size()) {
            next = *older_uncles.begin();
            break;
        }
        ancestor = ancestor->parent();
    }
    if (next) {
        double max = next->index() - COMMENT_GAP;
        Query younger = family_desc;
        younger.where("show_index < ?").bind(max);
        Comments younger_collection = younger.resultList();
        if (younger_collection.size()) {
            CommentPtr prev = *younger_collection.begin();
            if (abs(prev->index() - index()) > COMMENT_GAP) {
                index_ = (prev->index() + next->index()) / 2.0;
            }
        }
    } else {
        Comments family_desc_collection = family_desc.resultList();
        if (family_desc_collection.size()) {
            CommentPtr last = *family_desc_collection.begin();
            if (abs(last->index() - index()) > COMMENT_GAP) {
                index_ = last->index() + COMMENT_STEP;
            }
        } else {
            index_ = 1.0;
        }
    }
}

void Comment::set_type(Type type) {
    type_ = type;
    set_state(state_of_new(this->init(), this->type(), this->parent()));
}

Wt::WString Comment::text_or_removed(const UserPtr& viewer) const {
    if (type() == LOG_ENTRY && !viewer->has_permission(User::LOGS_READER)) {
        return "";
    }
    if (state() == OK) {
        return text();
    } else {
        Wt::WString result;
        if (state() == DELETED) {
            result = Wt::WString::tr("tc.comment.deleted_message");
        } else if (state() == DRAFT) {
            result = Wt::WString::tr("tc.comment.draft_message");
        }
        if (viewer && viewer->has_permission(User::COMMENTS_REMOVER)) {
            result += text();
        }
        return result;
    }
}

void Comment::set_text(const Wt::WString& text) {
    text_ = text;
    if (now() - edited_ > SECOND) {
        // edit, not create
        edited_ = now();
    }
}

void Comment::set_init(const UserPtr& init) {
    init_ = init;
    set_state(state_of_new(this->init(), this->type(), this->parent()));
}

void Comment::set_parent(const CommentPtr& parent, bool set_index) {
    if (!parent) {
        return;
    }
    parent_ = parent;
    depth_ = parent->depth() + 1;
    if (set_index) {
        this->set_index();
    }
    set_state(state_of_new(this->init(), this->type(), this->parent()));
}

void Comment::post_comment_added() {
    if (type() == FORUM_POST) {
        created_ = now();
    }
}

}

