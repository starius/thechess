/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MODEL_COMMENT_WIDGET_HPP_
#define THECHESS_MODEL_COMMENT_WIDGET_HPP_

#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

/** Viewing widget for a forum comment */
class CommentWidget : public Wt::WContainerWidget {
public:
    /** Constructor */
    CommentWidget(const CommentPtr& comment);
};

}

#endif

