/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WTextEdit>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>

#include "widgets/comment/ForumEdit.hpp"
#include "Application.hpp"
#include "utils/text_edit.hpp"

namespace thechess {

static void edit_comment(const CommentPtr& post_or_text,
                         Wt::WFormWidget* edit) {
    dbo::Transaction t(tApp->session());
    if (!post_or_text->can_edit(tApp->user())) {
        return;
    }
    if (edit->valueText().empty()) {
        return;
    }
    post_or_text.modify()->set_text(edit->valueText());
    post_or_text.modify()->set_state(Comment::state_of_new(tApp->user(),
                                     Comment::FORUM_COMMENT, post_or_text));
    CommentPtr post = post_or_text->type() == Comment::FORUM_POST ?
                      post_or_text : post_or_text->parent();
    t.commit();
    tApp->path().post()->set_integer_value(post.id());
    tApp->path().post()->open();
}

ForumEdit::ForumEdit(const CommentPtr& post_or_text, Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    if (!post_or_text->can_edit(tApp->user())) {
        return;
    }
    Wt::WFormWidget* edit;
    if (post_or_text->type() == Comment::FORUM_POST) {
        edit = new Wt::WLineEdit(this);
    } else {
        Wt::WTextEdit* text_edit = new Wt::WTextEdit(this);
        patch_text_edit(text_edit);
        edit = text_edit;
    }
    edit->setValueText(post_or_text->text());
    Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
    b->clicked().connect(boost::bind(edit_comment, post_or_text, edit));
}

}

