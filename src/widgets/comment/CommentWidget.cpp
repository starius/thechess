/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>

#include "widgets/comment/CommentWidget.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"

namespace thechess {

CommentWidget::CommentWidget(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    new Header(tr("tc.forum.Comment"), this);
    new Wt::WText(forum_comment_text(comment), this);
    t.commit();
}

}

