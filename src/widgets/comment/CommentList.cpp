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
#include <Wt/WCheckBox>
#include <Wt/WLineEdit>
#include <Wt/WTextEdit>
#include <Wt/WText>
#include <Wt/WAnchor>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/Pager.hpp>

#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/CommentModel.hpp"
#include "widgets/comment/remover_buttons.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/Header.hpp"
#include "model/global.hpp"
#include "Application.hpp"

namespace thechess {

const int COMMENT_WIDTH = 770;
const int COMMENT_HEIGHT = 450;
const int ID_WIDTH = 40;
const int CHAT_TIME_WIDTH = 40;
const int POST_TIME_WIDTH = 145;
const int INIT_WIDTH = 60;
const int COMMENT_ROW_HEIGHT_FORUM = 200;
const int COMMENT_CHAT_LENGTH = 80;
const int TOPIC_LENGTH = 80;
const int POST_LENGTH = 80;

class CommentList::CommentView : public Wt::WTableView {
public:
    CommentView(CommentModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        Comment::Type type = model->type();
        resize(COMMENT_WIDTH, COMMENT_HEIGHT);
        setColumnWidth(CommentModel::ID_COL, ID_WIDTH);
        setColumnWidth(CommentModel::INIT_COL, INIT_WIDTH);
        int comment_width = COMMENT_WIDTH - 30;
        if (type == Comment::FORUM_TOPIC) {
            setAlternatingRowColors(true);
            comment_width -= ID_WIDTH;
            setColumnHidden(CommentModel::TIME_COL, true);
            setColumnHidden(CommentModel::INIT_COL, true);
        } else if (type == Comment::FORUM_POST) {
            setAlternatingRowColors(true);
            comment_width -= ID_WIDTH;
            comment_width -= POST_TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, POST_TIME_WIDTH);
        } else if (type == Comment::CHAT_MESSAGE) {
            setAlternatingRowColors(true);
            setColumnHidden(CommentModel::ID_COL, true);
            comment_width -= CHAT_TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, CHAT_TIME_WIDTH);
        } else if (type == Comment::FORUM_COMMENT) {
            setRowHeight(COMMENT_ROW_HEIGHT_FORUM);
            addStyleClass("thechess-forum-comments");
            comment_width -= ID_WIDTH;
            setColumnHidden(CommentModel::TIME_COL, true);
            setColumnHidden(CommentModel::INIT_COL, true);
        }
        setColumnWidth(CommentModel::CONTENTS_COLUMN, comment_width);
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

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

CommentList::CommentList(Comment::Type type, const CommentPtr& root,
                         Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    Notifiable(Object(COMMENT, root.id()), tNot) {
    dbo::Transaction t(tApp->session());
    CommentModel* model = new CommentModel(type, root, this);
    view_ = new CommentView(model); // do it here to provide comment_model()
    print_header();
    if (type == Comment::FORUM_COMMENT && root) {
        CommentPtr post_text = root;
        CommentPtr post = root->parent();
        if (post->can_edit(tApp->user())) {
            Wt::WAnchor* e = new Wt::WAnchor(this);
            e->setLink(tApp->path().forum_edit()->get_link(post.id()));
            e->setText(tr("tc.forum.Edit"));
            new Wt::WBreak(this);
        }
    }
    if (type == Comment::FORUM_COMMENT && root) {
        CommentPtr post_text = root;
        new Wt::WText(post_text->text_or_removed(tApp->user()), this);
        new Wt::WBreak(this);
        if (post_text->can_edit(tApp->user())) {
            Wt::WAnchor* e = new Wt::WAnchor(this);
            e->setLink(tApp->path().forum_edit()->get_link(post_text.id()));
            e->setText(tr("tc.forum.Edit"));
            new Wt::WText(" ", this);
        }
        new Wt::WText(post_text->created().toString(), this);
        UserPtr user = post_text->init();
        if (user) {
            new Wt::WText(" ", this);
            user_anchor(user, this);
        }
        CommentPtr post = post_text->parent();
        if (post->edited() != post->created()) {
            new Wt::WBreak(this);
            new Wt::WText(tr("tc.forum.Post_edited")
                          .arg(post->edited().toString()), this);
        }
        if (post_text->edited() != post_text->created()) {
            new Wt::WBreak(this);
            new Wt::WText(tr("tc.forum.Post_text_edited")
                          .arg(post_text->edited().toString()), this);
        }
        new Wt::WBreak(this);
        new Wt::WBreak(this);
    }
    Wt::WCheckBox* only_ok = new Wt::WCheckBox(tr("tc.forum.Only_ok"), this);
    only_ok->changed().connect(
        boost::bind(&CommentModel::set_only_ok, model,
                    boost::bind(&Wt::WAbstractToggleButton::isChecked,
                                only_ok)));
    addWidget(view_);
    if (type == Comment::FORUM_TOPIC) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        line_edit->setTextSize(TOPIC_LENGTH);
        line_edit->setMaxLength(TOPIC_LENGTH);
    } else if (type == Comment::FORUM_POST) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        line_edit->setTextSize(POST_LENGTH);
        line_edit->setMaxLength(POST_LENGTH);
        post_text_ = new Wt::WTextEdit(this);
        Wt::Wc::fix_text_edit(post_text_);
    } else if (type == Comment::CHAT_MESSAGE) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        edit_->enterPressed().connect(boost::bind(&CommentList::add_comment,
                                      this, root));
        line_edit->setTextSize(COMMENT_CHAT_LENGTH);
        line_edit->setMaxLength(COMMENT_CHAT_LENGTH);
    } else if (type == Comment::FORUM_COMMENT) {
        Wt::WTextEdit* text_edit = new Wt::WTextEdit(this);
        edit_ = text_edit;
        Wt::Wc::fix_text_edit(text_edit);
    } else {
        // log error
    }
    if (Comment::can_create(tApp->user(), comment_model()->type(), root)) {
        Wt::WPushButton* add = new Wt::WPushButton(tr("tc.comment.Add"), this);
        add->clicked().connect(boost::bind(&CommentList::add_comment,
                                           this, root));
    }
    if (root && (root->type() == Comment::FORUM_POST_TEXT ||
                 root->type() == Comment::FORUM_TOPIC)) {
        add_remover_buttons(root, this);
    }
}

void CommentList::notify(EventPtr) {
    comment_model()->reload();
    view_->show_last();
}

CommentModel* CommentList::comment_model() const {
    return view_->comment_model();
}

void CommentList::print_header() {
    CommentPtr root = comment_model()->root();
    Comment::Type type = comment_model()->type();
    Wt::WString header;
    if (type == Comment::FORUM_TOPIC) {
        header = tr("tc.forum.topics");
    } else if (type == Comment::FORUM_POST) {
        if (root) {
            header = tr("tc.forum.topic_posts")
                     .arg(root->text_or_removed(tApp->user()));
        } else {
            header = tr("tc.forum.all_posts");
        }
    } else if (type == Comment::FORUM_COMMENT) {
        if (root) {
            CommentPtr post_text = root;
            CommentPtr post = root->parent();
            header = tr("tc.forum.post_header")
                     .arg(post.id()).arg(post->text_or_removed(tApp->user()));
        }
    }
    if (!header.empty()) {
        new Header(header, this);
    }
}

void CommentList::add_comment(const CommentPtr& parent) {
    Wt::WString text = edit_->valueText();
    Comment::Type type = comment_model()->type();
    if (text.empty()) {
        return;
    }
    if (type == Comment::FORUM_TOPIC && text.value().size() > TOPIC_LENGTH) {
        return;
    }
    if (type == Comment::FORUM_POST && text.value().size() > POST_LENGTH) {
        return;
    }
    if (type == Comment::CHAT_MESSAGE &&
            text.value().size() > COMMENT_CHAT_LENGTH) {
        return;
    }
    dbo::Transaction t(tApp->session());
    if (!Comment::can_create(tApp->user(), comment_model()->type(), parent)) {
        return;
    }
    CommentPtr comment = tApp->session().add(new Comment(true));
    comment.modify()->set_parent(parent);
    comment.modify()->set_type(type);
    comment.modify()->set_text(text);
    comment.modify()->set_init(tApp->user());
    CommentPtr root = comment_model()->root();
    comment.modify()->set_root(root);
    if (type == Comment::FORUM_POST) {
        comment.flush();
        CommentPtr post_text = tApp->session().add(new Comment(true));
        post_text.modify()->set_type(Comment::FORUM_POST_TEXT);
        post_text.modify()->set_parent(comment);
        post_text.modify()->set_text(post_text_->valueText());
        post_text.modify()->set_init(tApp->user());
    }
    if (type == Comment::FORUM_COMMENT) {
        // FORUM_POST's creation time is the time of last comment
        CommentPtr post = root->parent();
        post.modify()->post_comment_added();
    }
    t.commit();
    edit_->setValueText("");
    if (type == Comment::FORUM_POST) {
        post_text_->setValueText("");
    }
    tNot->emit(new Object(COMMENT, root.id()));
    if (!wApp->environment().ajax()) {
        comment_model()->reload();
    }
}

}

