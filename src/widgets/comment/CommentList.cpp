/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WEnvironment>
#include <Wt/WTableView>
#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WTextEdit>
#include <Wt/Wc/util.hpp>

#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/CommentModel.hpp"
#include "widgets/Header.hpp"
#include "model/global.hpp"
#include "Application.hpp"

namespace thechess {

const int COMMENT_WIDTH = 700;
const int COMMENT_ROW_HEIGHT_FORUM = 200;
const int COMMENT_CHAT_LENGTH = 80;
const int TOPIC_LENGTH = 80;
const int POST_LENGTH = 80;

class CommentList::CommentView : public Wt::WTableView {
public:
    CommentView(CommentModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(770, 450);
        setColumnWidth(CommentModel::CONTENTS_COLUMN, COMMENT_WIDTH);
        setHeaderHeight(0);
        show_last(); // FIXME has no effect in google chrome
    }

    void show_last() {
        if (model()->rowCount()) {
            int row = model()->rowCount() - 1;
            int column = CommentModel::CONTENTS_COLUMN;
            // FIXME http://redmine.emweb.be/issues/1380
            // scrollTo(model()->index(row, column));
        }
    }

    CommentModel* comment_model() const {
        return downcast<CommentModel*>(model());
    }
};

CommentList::CommentList(Comment::Type type, const CommentPtr& root,
                         Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    Notifiable(Object(COMMENT, root.id()), tNot) {
    dbo::Transaction t(tApp->session());
    Wt::WString header;
    if (type == Comment::FORUM_TOPIC) {
        header = tr("tc.forum.topics");
    } else if (type == Comment::FORUM_POST) {
        if (root) {
            header = tr("tc.forum.topic_posts").arg(root->text());
        } else {
            header = tr("tc.forum.all_posts");
        }
    } else if (type == Comment::FORUM_COMMENT) {
        if (root) {
            CommentPtr post_text = root;
            CommentPtr post = root->parent();
            header = tr("tc.forum.post_header")
                     .arg(post.id()).arg(post->text());
        }
    }
    if (!header.empty()) {
        new Header(header, this);
    }
    if (type == Comment::FORUM_COMMENT && root) {
        CommentPtr post_text = root;
        new Wt::WText(post_text->text(), this);
    }
    CommentModel* model = new CommentModel(type, root, this);
    view_ = new CommentView(model, this);
    if (type == Comment::FORUM_TOPIC) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        line_edit->setTextSize(TOPIC_LENGTH);
    } else if (type == Comment::FORUM_POST) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        line_edit->setTextSize(POST_LENGTH);
        post_text_ = new Wt::WTextEdit(this);
    } else if (type == Comment::CHAT_MESSAGE) {
        view_->setAlternatingRowColors(true);
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        edit_->enterPressed().connect(boost::bind(&CommentList::add_comment,
                                      this, root));
        line_edit->setTextSize(COMMENT_CHAT_LENGTH);
    } else if (type == Comment::FORUM_COMMENT) {
        view_->setRowHeight(COMMENT_ROW_HEIGHT_FORUM);
        edit_ = new Wt::WTextEdit(this);
    } else {
        // log error
    }
    Wt::WPushButton* add = new Wt::WPushButton(tr("tc.comment.Add"), this);
    add->clicked().connect(boost::bind(&CommentList::add_comment, this, root));
    t.commit();
}

void CommentList::notify(EventPtr) {
    comment_model()->reload();
    view_->show_last();
}

CommentModel* CommentList::comment_model() const {
    return view_->comment_model();
}

void CommentList::add_comment(const CommentPtr& parent) {
    if (edit_->valueText().empty()) {
        return;
    }
    dbo::Transaction t(tApp->session());
    CommentPtr comment = tApp->session().add(new Comment(true));
    if (parent) {
        comment.modify()->set_parent(parent);
        comment.modify()->set_type(Comment::child_type(parent->type()));
    } else {
        comment.modify()->set_type(comment_model()->type());
    }
    comment.modify()->set_text(edit_->valueText());
    comment.modify()->set_init(tApp->user());
    CommentPtr root = comment_model()->root();
    comment.modify()->set_root(root);
    if (comment_model()->type() == Comment::FORUM_POST) {
        comment.flush();
        CommentPtr post_text = tApp->session().add(new Comment(true));
        post_text.modify()->set_type(Comment::FORUM_POST_TEXT);
        post_text.modify()->set_parent(comment);
        post_text.modify()->set_text(post_text_->valueText());
        post_text.modify()->set_init(tApp->user());
    }
    t.commit();
    edit_->setValueText("");
    if (comment_model()->type() == Comment::FORUM_POST) {
        post_text_->setValueText("");
    }
    tNot->emit(new Object(COMMENT, root.id()));
    if (!wApp->environment().ajax()) {
        comment_model()->reload();
    }
}

}

