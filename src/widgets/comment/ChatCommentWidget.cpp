/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WPushButton>

#include "widgets/comment/ChatCommentWidget.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "widgets/comment/remover_buttons.hpp"
#include "widgets/comment/comment_base.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"

namespace thechess {

ChatCommentWidget::ChatCommentWidget(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() != Comment::CHAT_MESSAGE &&
            comment->type() != Comment::PRIVATE_MESSAGE) {
        return;
    }
    if (!comment->can_read_chat_logs(tApp->user())) {
        return;
    }
    new Header(tr("tc.forum.Comment"), this);
    new Wt::WText(forum_comment_text(comment), this);
    UserPtr user = comment->init();
    if (user) {
        new Wt::WBreak(this);
        user_anchor(user, this);
    }
    Wt::WAnchor* a = anchor_to_host(comment);
    if (a) {
        new Wt::WBreak(this);
        addWidget(a);
    }
    if (comment->type() == Comment::CHAT_MESSAGE) {
        add_remover_buttons(comment, this);
    }
}

}

