/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/TopBlock.hpp"
#include "Options.hpp"
#include "Application.hpp"

namespace thechess {

TopBlock::TopBlock(Wt::WContainerWidget* parent):
    WText(parent), Notifiable("motd") {
    update_contents();
}

void TopBlock::notify(EventPtr) {
    update_contents();
}

void TopBlock::update_contents() {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && !tApp->user()->email().empty()) {
        set_message();
    } else if (tApp->user()) {
        set_email_unverified();
    }
}

void TopBlock::set_message() {
    int comment_id = Options::instance()->top_logged_in_content_id();
    if (comment_id > 0) {
        dbo::Transaction t(tApp->session());
        try {
            CommentPtr comment = tApp->session().load<Comment>(comment_id,
                                 /* forceReread */ true);
            if (comment->type() == Comment::FORUM_COMMENT ||
                    comment->type() == Comment::FORUM_POST_TEXT) {
                setText(comment->text());
            }
        } catch (...)
        { }
    }
}

void TopBlock::set_email_unverified() {
    dbo::Transaction t(tApp->session());
    std::string unverified_email = "";
    if (tApp->user() && tApp->user()->auth_info()) {
        unverified_email = tApp->user()->auth_info()->unverifiedEmail();
    }
    std::string settings_path = tApp->path().settings_page()->full_path();
    setText(tr("tc.user.Email_unverified")
            .arg(unverified_email).arg(settings_path));
}

}

