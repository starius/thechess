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

#include "Application.hpp"
#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/CommentModel.hpp"

namespace thechess {

const int COMMENT_WIDTH = 700;
const int COMMENT_ROW_HEIGHT_FORUM = 200;
const int COMMENT_CHAT_LENGTH = 80;

class CommentList::CommentView : public Wt::WTableView {
public:
    CommentView(CommentModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(770, 450);
        setColumnWidth(CommentModel::CONTENTS_COLUMN, COMMENT_WIDTH);
        setHeaderHeight(0);
        show_last();
    }

    void show_last() {
        if (model()->rowCount()) {
            int row = model()->rowCount() - 1;
            int column = CommentModel::CONTENTS_COLUMN;
            scrollTo(model()->index(row, column));
        }
    }

    CommentModel* comment_model() const {
        return downcast<CommentModel*>(model());
    }
};

CommentList::CommentList(const CommentPtr& root, CommentType type,
                         Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    Notifiable(Object(COMMENT, root.id()), tNot),
    type_(type) {
    CommentModel* model = new CommentModel(root, this);
    view_ = new CommentView(model, this);
    if (type == CHAT) {
        view_->setAlternatingRowColors(true);
        Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
        edit_ = line_edit;
        edit_->enterPressed().connect(boost::bind(&CommentList::add_comment,
                                      this, root));
        line_edit->setTextSize(COMMENT_CHAT_LENGTH);
    } else {
        view_->setRowHeight(COMMENT_ROW_HEIGHT_FORUM);
        edit_ = new Wt::WTextEdit(this);
    }
    Wt::WPushButton* add = new Wt::WPushButton(tr("tc.comment.Add"), this);
    add->clicked().connect(boost::bind(&CommentList::add_comment, this, root));
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
    comment.modify()->set_parent(parent);
    comment.modify()->set_text(edit_->valueText());
    comment.modify()->set_init(tApp->user());
    if (type_ == CommentList::CHAT) {
        comment.modify()->set_commentable(false);
    }
    CommentPtr root = parent->root();
    t.commit();
    edit_->setValueText("");
    tNot->emit(new Object(COMMENT, root.id()));
    if (!wApp->environment().ajax()) {
        comment_model()->reload();
    }
}

}

