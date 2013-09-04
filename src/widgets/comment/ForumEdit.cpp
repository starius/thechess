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

static void edit_comment(CommentPtr c, Wt::WFormWidget* edit) {
    dbo::Transaction t(tApp->session());
    c.reread();
    if (!c->can_edit(tApp->user())) {
        return;
    }
    if (edit->valueText().empty()) {
        return;
    }
    c.modify()->set_text(patch_text_edit_text(edit->valueText()));
    c.modify()->set_state(Comment::state_of_new(tApp->user(),
                          c->type(), c));
    std::string url;
    if (c->type() == Comment::FORUM_POST) {
        url = tApp->path().post()->get_full_path(c.id());
    } else if (c->type() == Comment::FORUM_POST_TEXT) {
        url = tApp->path().post()->get_full_path(c->parent().id());
    } else if (c->type() == Comment::FORUM_COMMENT) {
        url = tApp->path().post()->get_full_path(c->root()->parent().id());
    } else if (c->type() == Comment::FORUM_TOPIC) {
        url = tApp->path().topic_posts()->get_full_path(c.id());
    }
    t.commit();
    wApp->internalPathChanged().emit(wApp->internalPath());
}

ForumEdit::ForumEdit(const CommentPtr& c, Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    dbo::Transaction t(tApp->session());
    if (!c->can_edit(tApp->user())) {
        return;
    }
    Wt::WFormWidget* edit;
    if (c->type() == Comment::FORUM_POST ||
            c->type() == Comment::FORUM_TOPIC) {
        edit = new Wt::WLineEdit(this);
    } else {
        Wt::WTextEdit* text_edit = new Wt::WTextEdit(this);
        patch_text_edit(text_edit);
        edit = text_edit;
    }
    edit->setValueText(c->text());
    Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Save"), this);
    b->clicked().connect(boost::bind(edit_comment, c, edit));
}

}

