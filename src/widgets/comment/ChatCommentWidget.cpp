/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WPushButton>

#include "widgets/comment/ChatCommentWidget.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "widgets/comment/remover_buttons.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"

namespace thechess {

ChatCommentWidget::ChatCommentWidget(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() != Comment::CHAT_MESSAGE) {
        return;
    }
    new Header(tr("tc.forum.Comment"), this);
    new Wt::WText(forum_comment_text(comment), this);
    UserPtr user = comment->init();
    if (user) {
        Wt::WAnchor* user_anchor = new Wt::WAnchor(this);
        user_anchor->setLink(tApp->path().user_view()->get_link(user.id()));
        user_anchor->setText(user->safe_username());
    }
    add_remover_buttons(comment, this);
    t.commit();
}

}

