/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/WTextEdit>
#include <Wt/WPushButton>

#include "widgets/comment/ForumCommentWidget.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "widgets/comment/remover_buttons.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "utils/text_edit.hpp"

namespace thechess {

static void add_comment(CommentPtr comment, Wt::WTextEdit* edit) {
    dbo::Transaction t(tApp->session());
    comment.reread();
    tApp->user().reread();
    if (!Comment::can_create(tApp->user(), Comment::FORUM_COMMENT, comment)) {
        return;
    }
    CommentPtr c = tApp->session().add(new Comment(true));
    c.modify()->set_parent(comment);
    c.modify()->set_type(Comment::FORUM_COMMENT);
    c.modify()->set_text(patch_text_edit_text(edit->valueText()));
    c.modify()->set_init(tApp->user());
    c.modify()->set_root(comment->root());
    int root_id = comment->root().id();
    CommentPtr post_text = comment->root();
    CommentPtr post = post_text->parent();
    post.reread();
    post.modify()->post_comment_added();
    t.commit();
    t_emit(COMMENT, root_id);
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
        new Wt::WBreak(this);
        user_anchor(user, this);
    }
    CommentPtr post_text = comment->root();
    CommentPtr post = post_text->parent();
    new Wt::WBreak(this);
    Wt::WAnchor* a = new Wt::WAnchor(this);
    a->setLink(tApp->path().post()->get_link(post.id()));
    a->setText(tr("tc.forum.post_header")
               .arg(post.id()).arg(post->text_or_removed(tApp->user())));
    CommentPtr parent = comment->parent();
    if (parent->type() == Comment::FORUM_COMMENT) {
        new Wt::WBreak(this);
        Wt::WAnchor* g = new Wt::WAnchor(this);
        g->setLink(tApp->path().post_comment()->get_link(parent.id()));
        g->setText(tr("tc.forum.Goto_parent").arg(parent.id()));
    }
    Wt::WTextEdit* edit = new Wt::WTextEdit(this);
    patch_text_edit(edit);
    new Wt::WBreak(this);
    if (Comment::can_create(tApp->user(), Comment::FORUM_COMMENT, comment)) {
        Wt::WPushButton* add = new Wt::WPushButton(tr("tc.comment.Add"), this);
        add->clicked().connect(boost::bind(add_comment, comment, edit));
    }
    add_remover_buttons(comment, this);
}

}

