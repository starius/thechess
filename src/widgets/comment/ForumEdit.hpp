/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_FORUM_EDIT_HPP_
#define THECHESS_WIDGETS_FORUM_EDIT_HPP_

#include <Wt/WContainerWidget>

#include "model/global.hpp"

namespace thechess {

/** Widget for editing my comments */
class ForumEdit : public Wt::WContainerWidget {
public:
    /** Constructor */
    ForumEdit(const CommentPtr& post_or_text, Wt::WContainerWidget* p = 0);
};

}

#endif

