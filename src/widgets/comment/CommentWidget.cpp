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
#include <Wt/Wc/util.hpp>

#include "widgets/comment/CommentWidget.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "widgets/comment/remover_buttons.hpp"
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
    tApp->setInternalPath(tApp->path().post()->get_full_path(post.id()), true);
}

CommentWidget::CommentWidget(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() != Comment::FORUM_POST_TEXT) {
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
    Wt::WTextEdit* edit = new Wt::WTextEdit(this);
    Wt::Wc::fix_text_edit(edit);
    if (Comment::can_create(tApp->user(), Comment::FORUM_COMMENT, comment)) {
        Wt::WPushButton* add = new Wt::WPushButton(tr("tc.comment.Add"), this);
        add->clicked().connect(boost::bind(add_comment, comment, edit));
    }
    add_remover_buttons(comment, this);
    t.commit();
}

}

