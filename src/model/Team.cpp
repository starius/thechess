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

Team::Team(bool)
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

}

