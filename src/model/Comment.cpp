/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cmath>

#include "model/all.hpp"

namespace thechess {

const double COMMENT_STEP = 1;
const double COMMENT_GAP = 0.000001;

Comment::Comment()
{ }

Comment::Comment(bool):
    index_(0),
    depth_(0),
    commentable_(true),
    state_(OK),
    created_(now()),
    edited_(now())
{ }

void Comment::set_index() {
    typedef dbo::Query<CommentPtr> Query;
    const Query family_desc = root_->family().find().orderBy("show_index desc");
    CommentPtr next;
    if (parent_->parent()) {
        Comments uncles = parent_->parent()->children();
        Query uncles_asc = uncles.find().orderBy("show_index");
        double min = parent_->index() + COMMENT_GAP;
        Comments older_uncles = uncles_asc.where("show_index > ?").bind(min);
        if (older_uncles.size()) {
            next = older_uncles.front();
        }
    }
    if (next) {
        double max = next->index() - COMMENT_GAP;
        Query younger = family_desc;
        younger.where("show_index < ?").bind(max);
        CommentPtr prev = younger.resultList().front();
        if (abs(prev->index() - index()) > COMMENT_GAP) {
            index_ = (prev->index() + next->index()) / 2.0;
        }
    } else {
        CommentPtr last = family_desc.resultList().front();
        if (abs(last->index() - index()) > COMMENT_GAP) {
            index_ = last->index() + COMMENT_STEP;
        }
    }
}

void Comment::set_text(const Wt::WString& text) {
    text_ = text;
    edited_ = now();
}

void Comment::set_parent(const CommentPtr& parent, bool set_index,
                         bool set_root) {
    parent_ = parent;
    depth_ = parent->depth();
    if (set_root) {
        root_ = parent->root();
    }
    if (set_index) {
        this->set_index();
    }
}

}

