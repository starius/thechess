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
#include <Wt/Wc/Pager.hpp>

#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/CommentModel.hpp"
#include "widgets/comment/remover_buttons.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/Header.hpp"
#include "model/global.hpp"
#include "Application.hpp"
#include "utils/text_edit.hpp"
#include "notify.hpp"

namespace thechess {

const int COMMENT_WIDTH = 770;
const int COMMENT_HEIGHT = 450;
const int ID_WIDTH = 40;
const int CHAT_TIME_WIDTH = 50;
const int LOG_TIME_WIDTH = 145;
const int POST_TIME_WIDTH = 145;
const int INIT_WIDTH = 60;
const int COMMENT_ROW_HEIGHT_FORUM = 200;
const int ALL_COMMENTS_ROW_HEIGHT = 40;
const int COMMENT_CHAT_LENGTH = 80;
const int TOPIC_LENGTH = 80;
const int POST_LENGTH = 80;
const int LOG_LENGTH = 1000;

class CommentList::CommentView : public Wt::WTableView, public Notifiable {
public:
    CommentView(CommentModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p),
        Notifiable(Object(COMMENT, model->root().id()), tNot) {
        setModel(model);
        Comment::Type type = model->type();
        int comment_width = COMMENT_WIDTH - 40;
        resize(COMMENT_WIDTH, COMMENT_HEIGHT);
        setColumnWidth(CommentModel::ID_COL, ID_WIDTH);
        setColumnWidth(CommentModel::INIT_COL, INIT_WIDTH);
        if (type == Comment::PRIVATE_MESSAGE) {
            setColumnWidth(CommentModel::DESTINATION_COL, INIT_WIDTH);
            comment_width -= INIT_WIDTH;
        } else {
            setColumnHidden(CommentModel::DESTINATION_COL, true);
        }
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
        } else if (type == Comment::NO_TYPE) {
            setRowHeight(ALL_COMMENTS_ROW_HEIGHT);
            setAlternatingRowColors(true);
            setColumnHidden(CommentModel::ID_COL, true);
            comment_width -= POST_TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, POST_TIME_WIDTH);
        } else if (type == Comment::CHAT_MESSAGE) {
            setAlternatingRowColors(true);
            setColumnHidden(CommentModel::ID_COL, true);
            comment_width -= CHAT_TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, CHAT_TIME_WIDTH);
        } else if (type == Comment::LOG_ENTRY ||
                   type == Comment::PRIVATE_MESSAGE) {
            setAlternatingRowColors(true);
            setColumnHidden(CommentModel::ID_COL, true);
            comment_width -= LOG_TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, LOG_TIME_WIDTH);
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

    void notify(EventPtr) {
        comment_model()->reload();
        show_last();
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

CommentList::CommentList(Comment::Type type, const CommentPtr& root,
                         const UserPtr& init, Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    edit_(0) {
    dbo::Transaction t(tApp->session());
    if (type == Comment::LOG_ENTRY) {
        if (!tApp->user() || !tApp->user()->has_permission(LOGS_READER)) {
            return;
        }
    }
    if (type == Comment::PRIVATE_MESSAGE && !tApp->user()) {
        return;
    }
    if (type != Comment::NO_TYPE && root &&
            Comment::root_type(type) != root->type()) {
        return;
    }
    CommentModel* model = new CommentModel(type, root, init, this);
    view_ = new CommentView(model); // do it here to provide comment_model()
    print_header();
    if (type == Comment::FORUM_COMMENT && root) {
        print_post();
    }
    Wt::WCheckBox* only_ok = new Wt::WCheckBox(tr("tc.forum.Only_ok"), this);
    only_ok->setChecked(model->only_ok());
    only_ok->changed().connect(
        boost::bind(&CommentModel::set_only_ok, model,
                    boost::bind(&Wt::WAbstractToggleButton::isChecked,
                                only_ok)));
    if (tApp->user() && !init) {
        Wt::WCheckBox* only_my = new Wt::WCheckBox(this);
        only_my->setChecked(model->only_my());
        only_my->setText(tr("tc.common.Only_my"));
        only_my->changed().connect(
            boost::bind(&CommentModel::set_only_my, model,
                        boost::bind(&Wt::WAbstractToggleButton::isChecked,
                                    only_my)));
    }
    addWidget(view_);
    if (Comment::can_create(tApp->user(), type, root)) {
        print_edits();
        if (edit_) {
            Wt::WPushButton* add;
            add = new Wt::WPushButton(tr("tc.comment.Add"), this);
            add->clicked().connect(boost::bind(&CommentList::add_comment,
                                               this, root));
        }
    }
    if (root && (root->type() == Comment::FORUM_POST_TEXT ||
                 root->type() == Comment::FORUM_TOPIC)) {
        add_remover_buttons(root, this);
    }
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
    } else if (type == Comment::LOG_ENTRY) {
        header = tr("tc.comment.admin_log");
    } else if (type == Comment::PRIVATE_MESSAGE) {
        header = tr("tc.comment.private_messages");
    } else if (type == Comment::CHAT_MESSAGE) {
        header = tr("tc.comment.Global_chat");
    }
    if (!header.empty()) {
        new Header(header, this);
    }
}

void CommentList::print_post() {
    CommentPtr post_text = comment_model()->root();
    CommentPtr post = post_text->parent();
    if (post->can_edit(tApp->user())) {
        Wt::WAnchor* e = new Wt::WAnchor(this);
        e->setLink(tApp->path().forum_edit()->get_link(post.id()));
        e->setText(tr("tc.forum.Edit"));
        new Wt::WBreak(this);
    }
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
    CommentPtr topic = post->parent();
    if (topic) {
        new Wt::WBreak(this);
        Wt::WAnchor* a = new Wt::WAnchor(this);
        a->setLink(tApp->path().topic_posts()->get_link(topic.id()));
        a->setText(tr("tc.forum.topic_posts")
                   .arg(topic->text_or_removed(tApp->user())));
    }
    new Wt::WBreak(this);
    new Wt::WBreak(this);
}

void CommentList::add_comment(const CommentPtr& parent) {
    if (!edit_) {
        return;
    }
    Comment::Type type = comment_model()->type();
    Wt::WString text = edit_->valueText();
    if (type == Comment::FORUM_COMMENT) {
        text = patch_text_edit_text(text);
    }
    if (text.empty()) {
        return;
    }
    if (type == Comment::FORUM_TOPIC && text.value().size() > TOPIC_LENGTH) {
        return;
    }
    if (type == Comment::FORUM_POST && text.value().size() > POST_LENGTH) {
        return;
    }
    if (type == Comment::LOG_ENTRY && text.value().size() > LOG_LENGTH) {
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
        Wt::WString description = post_text_->valueText();
        post_text.modify()->set_text(patch_text_edit_text(description));
        post_text.modify()->set_init(tApp->user());
    }
    if (type == Comment::FORUM_COMMENT) {
        // FORUM_POST's creation time is the time of last comment
        CommentPtr post = root->parent();
        post.reread();
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

void CommentList::print_edits() {
    CommentPtr root = comment_model()->root();
    Comment::Type type = comment_model()->type();
    if (type == Comment::FORUM_TOPIC) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        line_edit->setTextSize(80);
        line_edit->setMaxLength(TOPIC_LENGTH);
    } else if (type == Comment::FORUM_POST) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        line_edit->setTextSize(80);
        line_edit->setMaxLength(POST_LENGTH);
        post_text_ = new Wt::WTextEdit(this);
        patch_text_edit(post_text_);
        new Wt::WBreak(this);
    } else if (type == Comment::CHAT_MESSAGE) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        edit_->enterPressed().connect(boost::bind(&CommentList::add_comment,
                                      this, root));
        line_edit->setTextSize(80);
        line_edit->setMaxLength(COMMENT_CHAT_LENGTH);
    } else if (type == Comment::FORUM_COMMENT) {
        Wt::WTextEdit* text_edit = new Wt::WTextEdit(this);
        patch_text_edit(text_edit);
        new Wt::WBreak(this);
        edit_ = text_edit;
    } else if (type == Comment::LOG_ENTRY) {
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        line_edit->setTextSize(80);
        line_edit->setMaxLength(LOG_LENGTH);
    }
    // PRIVATE_MESSAGE has no 'Add' feature.
    // see UserWidget
}

}

