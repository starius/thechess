/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "model/all.hpp"
#include "Options.hpp"

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
    edited_(created_),
    score_(0) {
    if (wApp) {
        ip_ = wApp->environment().clientAddress();
    }
}

Comment::Type Comment::child_type(Comment::Type type) {
    if (type == CHAT_ROOT) {
        return CHAT_MESSAGE; // FIXME PRIVATE_MESSAGE also match
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
    if (type == CHAT_MESSAGE || type == PRIVATE_MESSAGE) {
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
        if (type == CHAT_MESSAGE && !user->has_permission(CHAT_WRITER)) {
            return DELETED;
        }
        if (type == FORUM_TOPIC && !user->has_permission(COMMENTS_REMOVER)) {
            return DELETED;
        }
        if (type == FORUM_POST && !user->has_permission(FORUM_POST_CREATOR)) {
            return DELETED;
        }
        if (type == FORUM_COMMENT &&
                !user->has_permission(FORUM_COMMENT_CREATOR)) {
            return DELETED;
        }
        if (type == LOG_ENTRY && !user->has_permission(LOGS_READER)) {
            return DELETED;
        }
        if (type == PRIVATE_MESSAGE && !user->has_permission(PRIVATE_WRITER)) {
            return DELETED;
        }
        return OK;
    } else {
        UserRights anon_rights = Options::instance()->anonymous_rights();
        if (IpBan::am_i_banned()) {
            int a = anon_rights & Options::instance()->banned_ip_user_rights();
            anon_rights = UserRights(a);
        }
        if (type == CHAT_MESSAGE &&
                User::has_permission(CHAT_WRITER, anon_rights)) {
            return DRAFT;
        }
        if (type == FORUM_COMMENT &&
                User::has_permission(FORUM_COMMENT_CREATOR, anon_rights)) {
            return DRAFT;
        }
        if (type == FORUM_POST &&
                User::has_permission(FORUM_POST_CREATOR, anon_rights)) {
            return DRAFT;
        }
        return DELETED;
    }
}

bool Comment::can_create(const UserPtr& user, Type type,
                         const CommentPtr& parent) {
    return state_of_new(user, type, parent) != DELETED;
}

bool Comment::can_edit(const UserPtr& user) const {
    if (type() != FORUM_POST && type() != FORUM_POST_TEXT &&
            type() != FORUM_COMMENT && type() != FORUM_TOPIC) {
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
    CommentPtr r = root();
    if (!r || !parent_) {
        return;
    }
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
            if (abs(last->index() - index()) > COMMENT_GAP ||
                    index() < COMMENT_GAP) {
                index_ = last->index() + COMMENT_STEP;
            }
        } else {
            index_ = 1.0;
        }
    }
}

void Comment::set_depth() {
    if (parent()) {
        depth_ = parent()->depth() + 1;
    }
}

void Comment::set_type(Type type) {
    type_ = type;
}

bool Comment::can_read_chat_logs(const UserPtr& viewer) const {
    if (type() != PRIVATE_MESSAGE && type() != LOG_ENTRY) {
        return true;
    }
    if (type() == LOG_ENTRY && (!viewer ||
                                !viewer->has_permission(LOGS_READER))) {
        return false;
    }
    if (type() == PRIVATE_MESSAGE && viewer != init()) {
        if (!parent() || !viewer || !viewer->has_comment_base() ||
                viewer->comment_base() != parent()) {
            return false;
        }
    }
    return true;
}

Wt::WString Comment::text_or_removed(const UserPtr& viewer) const {
    if (type() == PRIVATE_MESSAGE || type() == LOG_ENTRY) {
        if (!can_read_chat_logs(viewer)) {
            return "";
        }
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
        if (viewer) {
            if (viewer->has_permission(COMMENTS_REMOVER) ||
                    init() == viewer || viewer->comment_base() == self()) {
                result += text();
            }
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
}

void Comment::set_root(const CommentPtr& root) {
    root_ = root;
    set_index();
}

void Comment::set_parent(const CommentPtr& parent) {
    if (!parent) {
        return;
    }
    parent_ = parent;
    set_depth();
    set_index();
}

void Comment::post_comment_added() {
    if (type() == FORUM_POST) {
        created_ = now();
    }
}

}

