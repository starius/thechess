/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WPushButton>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/Wc/util.hpp>

#include "model/all.hpp"
#include "Application.hpp"
#include "widgets/user/NewIpBan.hpp"
#include "log.hpp"

namespace thechess {

static void change_state(CommentPtr comment, Comment::State state) {
    dbo::Transaction t(tApp->session());
    comment.reread();
    if (tApp->user() && tApp->user()->has_permission(COMMENTS_REMOVER)) {
        comment.modify()->set_state(state);
        if (comment->type() == Comment::FORUM_POST_TEXT) {
            CommentPtr post = comment->parent();
            post.reread();
            post.modify()->set_state(state);
        }
        // admin log
        Wt::WString action = state == Comment::OK ? "Approve " : "Remove ";
        admin_log(action + comm_a(comment.id()));
    }
    t.commit();
    tApp->path().open(tApp->internalPath());
}

static void show_new_ban(Wt::WPushButton* b, CommentPtr c) {
    dbo::Transaction t(tApp->session());
    Wt::WContainerWidget* parent = DOWNCAST<Wt::WContainerWidget*>(b->parent());
    delete b;
    parent->addWidget(new NewIpBan(c->ip(), comm_a(c.id())));
}

void add_remover_buttons(const CommentPtr& comment, Wt::WContainerWidget* p) {
    dbo::Transaction t(tApp->session());
    if (tApp->user() && tApp->user()->has_permission(COMMENTS_REMOVER)) {
        p->addWidget(new Wt::WBreak());
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
    if (tApp->user() && tApp->user()->has_permission(REGISTRATION_BANNER)) {
        p->addWidget(new Wt::WBreak());
        Wt::WAnchor* a = new Wt::WAnchor();
        a->setText(comment->ip());
        a->setLink(tApp->path().banned_ip()->get_link(comment->ip()));
        p->addWidget(a);
        Wt::WPushButton* b;
        b = new Wt::WPushButton(Wt::WString::tr("tc.user.New_ban"), p);
        b->clicked().connect(boost::bind(show_new_ban, b, comment));
    }
}

}

