/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WPushButton>

#include "model/all.hpp"
#include "Application.hpp"
#include "log.hpp"

namespace thechess {

static void change_state(CommentPtr comment, Comment::State state) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && tApp->user()->has_permission(User::COMMENTS_REMOVER)) {
        comment.modify()->set_state(state);
        if (comment->type() == Comment::FORUM_POST_TEXT) {
            CommentPtr post = comment->parent();
            post.modify()->set_state(state);
        }
        // admin log
        Wt::Wc::url::IntegerNode* node = 0;
        Wt::WString text;
        int id;
        if (comment->type() == Comment::CHAT_MESSAGE) {
            node = tApp->path().chat_comment();
            text = "chat message {1}";
            id = comment.id();
        } else if (comment->type() == Comment::FORUM_POST_TEXT) {
            node = tApp->path().post();
            text = "forum post {1}";
            id = comment->parent().id();
        } else if (comment->type() == Comment::FORUM_TOPIC) {
            node = tApp->path().topic_posts();
            text = "forum topic {1}";
            id = comment.id();
        } else if (comment->type() == Comment::FORUM_COMMENT) {
            node = tApp->path().post_comment();
            text = "forum comment {1}";
            id = comment.id();
        }
        if (node) {
            text.arg(id);
            Wt::WString action = state == Comment::OK ? "Approve " : "Remove ";
            admin_log(html_a(node, id, action + text));
        }
    }
    t.commit();
    tApp->path().open(tApp->internalPath());
}

void add_remover_buttons(const CommentPtr& comment, Wt::WContainerWidget* p) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && tApp->user()->has_permission(User::COMMENTS_REMOVER)) {
        Wt::WPushButton* b;
        if (comment->state() != Comment::OK) {
            b = new Wt::WPushButton(Wt::WString::tr("tc.comment.Approve"), p);
            b->clicked().connect(boost::bind(change_state,
                                             comment, Comment::OK));
        }
        if (comment->state() != Comment::DELETED) {
            b = new Wt::WPushButton(Wt::WString::tr("tc.comment.Remove"), p);
            b->clicked().connect(boost::bind(change_state,
                                             comment, Comment::DELETED));
        }
    }
}

}

