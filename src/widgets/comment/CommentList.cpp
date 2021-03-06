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
#include <Wt/Wc/util.hpp>

#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/CommentModel.hpp"
#include "widgets/comment/comment_base.hpp"
#include "widgets/comment/remover_buttons.hpp"
#include "widgets/user/user_anchor.hpp"
#include "widgets/Header.hpp"
#include "model/global.hpp"
#include "Application.hpp"
#include "utils/text_edit.hpp"
#include "log.hpp"

namespace thechess {

const int COMMENT_WIDTH = 770;
const int COMMENT_HEIGHT = 700;
const int ID_WIDTH = 80;
const int NO_TYPE_ID_WIDTH = 110;
const int TIME_WIDTH = 50;
const int INIT_WIDTH = 120;
const int COMMENT_ROW_HEIGHT_FORUM = 20;
const int ALL_COMMENTS_ROW_HEIGHT = 35;
const int COMMENT_CHAT_LENGTH = 10000;
const int TOPIC_LENGTH = 80;
const int POST_LENGTH = 80;
const int LOG_LENGTH = 1000;

class CommentList::CommentView : public Wt::WTableView, public Notifiable {
public:
    CommentView(CommentModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p),
        Notifiable(Object(COMMENT, model->root().id())) {
        setModel(model);
        Comment::Type type = model->type();
        int comment_width = COMMENT_WIDTH - 40;
        int id_width = (type == Comment::NO_TYPE) ? NO_TYPE_ID_WIDTH : ID_WIDTH;
        resize(COMMENT_WIDTH, COMMENT_HEIGHT);
        setSelectable(true);
        setColumnWidth(CommentModel::ID_COL, id_width);
        setColumnWidth(CommentModel::INIT_COL, INIT_WIDTH);
        if (type == Comment::PRIVATE_MESSAGE) {
            setColumnWidth(CommentModel::DESTINATION_COL, INIT_WIDTH);
            comment_width -= INIT_WIDTH;
        } else {
            setColumnHidden(CommentModel::DESTINATION_COL, true);
        }
        if (type == Comment::FORUM_TOPIC) {
            setAlternatingRowColors(true);
            setColumnHidden(CommentModel::ID_COL, true);
            setColumnHidden(CommentModel::TIME_COL, true);
            setColumnHidden(CommentModel::INIT_COL, true);
        } else if (type == Comment::FORUM_POST) {
            setAlternatingRowColors(true);
            comment_width -= id_width;
            comment_width -= TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, TIME_WIDTH);
        } else if (type == Comment::NO_TYPE) {
            setRowHeight(ALL_COMMENTS_ROW_HEIGHT);
            setAlternatingRowColors(true);
            comment_width -= id_width;
            comment_width -= TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, TIME_WIDTH);
        } else if (type == Comment::CHAT_MESSAGE) {
            addStyleClass("thechess-chat");
            setColumnHidden(CommentModel::ID_COL, true);
            comment_width -= TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, TIME_WIDTH);
        } else if (type == Comment::LOG_ENTRY) {
            setColumnHidden(CommentModel::ID_COL, true);
            comment_width -= TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, TIME_WIDTH);
            addStyleClass("thechess-private-messages");
        } else if (type == Comment::PRIVATE_MESSAGE) {
            setColumnHidden(CommentModel::ID_COL, true);
            comment_width -= TIME_WIDTH;
            comment_width -= INIT_WIDTH;
            setColumnWidth(CommentModel::TIME_COL, TIME_WIDTH);
            addStyleClass("thechess-private-messages");
        } else if (type == Comment::FORUM_COMMENT) {
            setRowHeight(COMMENT_ROW_HEIGHT_FORUM);
            addStyleClass("thechess-forum-comments");
            comment_width -= id_width;
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
            Wt::Wc::scroll_to_last(this);
        }
    }

    CommentModel* comment_model() const {
        return downcast<CommentModel*>(model());
    }

    void notify(EventPtr) {
        comment_model()->reload();
        if (comment_model()->type() != Comment::FORUM_COMMENT) {
            show_last();
        }
    }

protected:
    WWidget* createPageNavigationBar() {
        Wt::Wc::Pager* pager = new Wt::Wc::Pager(this);
        pager->set_base(5);
        pager->set_half_of_frame(2);
        return pager;
    }
};

CommentList::CommentList(Comment::Type type, const CommentPtr& root,
                         const UserPtr& init, Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    edit_(0), only_ok_(0), only_draft_(0), only_my_(0), init_(init) {
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
    if (type == Comment::FORUM_POST && root) {
        // edit this topic
        if (root->can_edit(tApp->user())) {
            Wt::WAnchor* e = new Wt::WAnchor(this);
            e->setLink(tApp->path().forum_edit()->get_link(root.id()));
            e->setText(tr("tc.forum.Edit"));
            new Wt::WBreak(this);
        }
    }
    if (type == Comment::FORUM_COMMENT && root) {
        print_post();
    }
    only_ok_ = new Wt::WCheckBox(tr("tc.forum.Only_ok"), this);
    only_ok_->setChecked(model->only_ok());
    only_ok_->changed().connect(this, &CommentList::apply);
    if (tApp->user() && tApp->user()->has_permission(COMMENTS_REMOVER)) {
        only_draft_ = new Wt::WCheckBox(tr("tc.comment.draft_message"), this);
        only_draft_->setChecked(model->only_draft());
        only_draft_->changed().connect(this, &CommentList::apply);
    }
    if (tApp->user() && !init) {
        only_my_ = new Wt::WCheckBox(this);
        only_my_->setChecked(model->only_my());
        only_my_->setText(tr("tc.common.Only_my"));
        only_my_->changed().connect(this, &CommentList::apply);
    }
    text_like_ = new Wt::WLineEdit(this);
    text_like_->setEmptyText(tr("tc.comment.Text"));
    text_like_->enterPressed().connect(this, &CommentList::apply);
    if (!wApp->environment().ajax()) {
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Apply"), this);
        b->clicked().connect(this, &CommentList::apply);
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

void CommentList::set_ip(const std::string ip) {
    if (!tApp->user() || !tApp->user()->has_permission(REGISTRATION_BANNER)) {
        return;
    }
    comment_model()->set_ip(ip);
    Wt::Wc::scroll_to_last(view_);
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
    new Wt::WBreak(this);
    new Wt::WBreak(this);
    new Wt::WText(time2str(post_text->created()), this);
    UserPtr user = post_text->init();
    if (user) {
        new Wt::WText(" ", this);
        user_anchor(user, this);
    }
    if (post->edited() != post_text->created()) {
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.forum.Post_edited")
                      .arg(time2str(post->edited())), this);
    }
    if (post_text->edited() != post_text->created()) {
        new Wt::WBreak(this);
        new Wt::WText(tr("tc.forum.Post_text_edited")
                      .arg(time2str(post_text->edited())), this);
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

void CommentList::apply() {
    if (only_my_) {
        comment_model()->set_only_my(only_my_->isChecked());
    }
    if (only_ok_) {
        comment_model()->set_only_ok(only_ok_->isChecked());
    }
    if (only_draft_) {
        comment_model()->set_only_draft(only_draft_->isChecked());
    }
    comment_model()->set_text_like(text_like_->text());
    view_->show_last();
}

void CommentList::add_comment(const CommentPtr& parent) {
    if (!edit_) {
        return;
    }
    Comment::Type type = comment_model()->type();
    Wt::WString text = edit_->valueText();
    if (Wt::Wc::isinstance<Wt::WTextEdit>(edit_)) {
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
    tApp->user().reread();
    if (type == Comment::PRIVATE_MESSAGE) {
        if (init_) {
            const UserPtr& he = init_;
            UserPtr me = tApp->user();
            if (me && me->can_send_message(he)) {
                User::send_message(me, he, text);
                edit_->setValueText("");
            }
        }
        return;
    }
    Comment::State state = Comment::state_of_new(tApp->user(),
                           comment_model()->type(), parent);
    if (state == Comment::DELETED) {
        return;
    }
    CommentPtr comment = tApp->session().add(new Comment(true));
    comment.modify()->set_parent(parent);
    comment.modify()->set_type(type);
    comment.modify()->set_text(text);
    comment.modify()->set_init(tApp->user());
    comment.modify()->set_state(state);
    CommentPtr root = comment_model()->root();
    comment.modify()->set_root(root);
    if (type == Comment::FORUM_POST) {
        comment.flush();
        admin_log("New post " + comm_a(comment.id()));
        CommentPtr post_text = tApp->session().add(new Comment(true));
        post_text.modify()->set_created(comment->created());
        post_text.modify()->set_edited(comment->created());
        post_text.modify()->set_type(Comment::FORUM_POST_TEXT);
        post_text.modify()->set_parent(comment);
        Wt::WString description = post_text_->valueText();
        post_text.modify()->set_text(patch_text_edit_text(description));
        post_text.modify()->set_init(tApp->user());
        post_text.modify()->set_state(state);
    }
    if (type == Comment::FORUM_COMMENT) {
        // FORUM_POST's creation time is the time of last comment
        CommentPtr post = root->parent();
        post.reread();
        post.modify()->post_comment_added();
        // private notification
        User::send_message(tApp->user(), post->init(),
                           "[auto] " + comm_a(comment.id()));
    }
    t.commit();
    edit_->setValueText("");
    if (type == Comment::FORUM_POST) {
        post_text_->setValueText("");
    }
    t_emit(COMMENT, root.id());
    Object base = comment_base(comment);
    if (base.type == GAME) {
        t_emit(GAME, base.id);
    }
    if (state == Comment::DRAFT) {
        clear();
        addWidget(new Wt::WText(tr("tc.comment.draft_message")));
    } else if (!wApp->environment().ajax()) {
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
        if (!User::has_s(SWITCH_FORMATTING_CHAT) ||
                // FIXME maybe http://redmine.emweb.be/issues/1494
                comment_base(root).type == COMPETITION) {
            Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
            edit_ = line_edit;
            edit_->enterPressed().connect(boost::bind(&CommentList::add_comment,
                                          this, root));
            line_edit->setTextSize(80);
            line_edit->setMaxLength(COMMENT_CHAT_LENGTH);
        } else {
            Wt::WTextEdit* text_edit = new Wt::WTextEdit(this);
            patch_text_edit(text_edit);
            new Wt::WBreak(this);
            edit_ = text_edit;
        }
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
    } else if (type == Comment::PRIVATE_MESSAGE && init_ &&
               tApp->user() && tApp->user()->can_send_message(init_)) {
        const UserPtr& he = init_;
        CommentPtr his_base = he->comment_base();
        if (!User::has_s(SWITCH_FORMATTING_CHAT)) {
            Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
            line_edit->setTextSize(80);
            edit_ = line_edit;
            edit_->enterPressed().connect(boost::bind(&CommentList::add_comment,
                                          this, CommentPtr()));
        } else {
            Wt::WTextEdit* text_edit = new Wt::WTextEdit(this);
            patch_text_edit(text_edit);
            new Wt::WBreak(this);
            edit_ = text_edit;
        }
    }
}

}

