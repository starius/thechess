/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WPushButton>

#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

static void change_state(CommentPtr comment, Comment::State state) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && tApp->user()->has_permission(User::COMMENTS_REMOVER)) {
        comment.modify()->set_state(state);
        if (comment->type() == Comment::FORUM_POST_TEXT) {
            CommentPtr post = comment->parent();
            post.modify()->set_state(state);
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

