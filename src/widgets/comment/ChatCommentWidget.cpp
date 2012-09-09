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
#include "widgets/user/user_anchor.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"

namespace thechess {

Wt::WAnchor* anchor_to_host(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() != Comment::CHAT_MESSAGE) {
        return 0;
    }
    if (!comment->root()) {
        Wt::WAnchor* a = new Wt::WAnchor();
        a->setText(Wt::WString::tr("tc.comment.Global_chat"));
        a->setLink(tApp->path().global_chat()->link());
        return a;
    }
    try {
        GamePtr game = tApp->session().find<Game>()
                       .where("comment_base_id = ?").bind(comment->root());
        if (game) {
            Wt::WAnchor* a = new Wt::WAnchor();
            a->setText(Wt::WString::tr("tc.game.Header").arg((int)game.id()));
            a->setLink(tApp->path().game_view()->get_link(game.id()));
            return a;
        }
    } catch (...)
    { }
    try {
        CompetitionPtr c = tApp->session().find<Competition>()
                           .where("comment_base_id = ?").bind(comment->root());
        if (c) {
            Wt::WAnchor* a = new Wt::WAnchor();
            a->setText(Wt::WString::tr("tc.competition.Header")
                       .arg((int)c.id()));
            a->setLink(tApp->path().competition_view()->get_link(c.id()));
            return a;
        }
    } catch (...)
    { }
    return 0;
}

ChatCommentWidget::ChatCommentWidget(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() != Comment::CHAT_MESSAGE &&
            comment->type() != Comment::PRIVATE_MESSAGE) {
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

