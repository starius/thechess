/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WText>
#include <Wt/WTextEdit>
#include <Wt/WPushButton>
#include <Wt/Wc/util.hpp>

#include "widgets/comment/ForumCommentWidget.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "widgets/comment/remover_buttons.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"

namespace thechess {

static void add_comment(const CommentPtr& comment, Wt::WTextEdit* edit) {
    dbo::Transaction t(tApp->session());
    if (!Comment::can_create(tApp->user(), Comment::FORUM_COMMENT, comment)) {
        return;
    }
    CommentPtr c = tApp->session().add(new Comment(true));
    c.modify()->set_parent(comment);
    c.modify()->set_type(Comment::FORUM_COMMENT);
    c.modify()->set_text(edit->valueText());
    c.modify()->set_init(tApp->user());
    c.modify()->set_root(comment->root());
    CommentPtr post_text = comment->root();
    CommentPtr post = post_text->parent();
    post.modify()->post_comment_added();
    t.commit();
    tApp->path().post()->set_integer_value(post.id());
    tApp->path().post()->open();
}

ForumCommentWidget::ForumCommentWidget(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() != Comment::FORUM_COMMENT) {
        return;
    }
    new Header(tr("tc.forum.Comment"), this);
    new Wt::WText(forum_comment_text(comment), this);
    UserPtr user = comment->init();
    if (user) {
        user_anchor(user, this);
    }
    Wt::WTextEdit* edit = new Wt::WTextEdit(this);
    Wt::Wc::fix_text_edit(edit);
    if (Comment::can_create(tApp->user(), Comment::FORUM_COMMENT, comment)) {
        Wt::WPushButton* add = new Wt::WPushButton(tr("tc.comment.Add"), this);
        add->clicked().connect(boost::bind(add_comment, comment, edit));
    }
    add_remover_buttons(comment, this);
}

}

