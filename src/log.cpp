/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WString>

#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

void admin_log(const Wt::WString& message) {
    dbo::Transaction t(tApp->session());
    CommentPtr comment = tApp->session().add(new Comment(true));
    comment.modify()->set_type(Comment::LOG_ENTRY);
    comment.modify()->set_init(tApp->user());
    comment.modify()->set_text(message);
}

}

