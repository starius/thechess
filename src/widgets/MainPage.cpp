/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/MainPage.hpp"
#include "Options.hpp"
#include "Application.hpp"

namespace thechess {

MainPage::MainPage(Wt::WContainerWidget* parent):
    WText(parent) {
    int comment_id = Options::instance()->main_page_content_id();
    if (comment_id > 0) {
        dbo::Transaction t(tApp->session());
        try {
            CommentPtr comment = tApp->session().load<Comment>(comment_id);
            setText(comment->text());
        } catch (...)
        { }
    }
    setText(tr("tc.main.main"));
}

}

