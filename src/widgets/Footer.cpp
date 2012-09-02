/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>

#include "widgets/Footer.hpp"
#include "Options.hpp"
#include "Application.hpp"

namespace thechess {

Footer::Footer(Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    int comment_id = Options::instance()->footer_content_id();
    if (comment_id > 0) {
        dbo::Transaction t(tApp->session());
        CommentPtr comment = tApp->session().load<Comment>(comment_id);
        addWidget(new Wt::WText(comment->text()));
    }
}

}

