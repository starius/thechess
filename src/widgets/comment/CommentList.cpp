/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTableView>

#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/CommentModel.hpp"

namespace thechess {

const int COMMENT_WIDTH = 700;
const int COMMENT_HEIGHT = 200;

class CommentView : public Wt::WTableView {
public:
    CommentView(CommentModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(770, 450);
        setColumnWidth(CommentModel::CONTENTS_COLUMN, COMMENT_WIDTH);
        setRowHeight(COMMENT_HEIGHT);
        setHeaderHeight(0);
        if (model->rowCount()) {
            int row = model->rowCount() - 1;
            int column = CommentModel::CONTENTS_COLUMN;
            scrollTo(model->index(row, column));
        }
    }
};

CommentList::CommentList(const CommentPtr& root, Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent) {
    CommentModel* model = new CommentModel(root, this);
    new CommentView(model, this);
}

}

