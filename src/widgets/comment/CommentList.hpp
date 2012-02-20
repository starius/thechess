/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMMENT_LIST_HPP_
#define THECHESS_WIDGETS_COMMENT_LIST_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

/** Widget containing a list of comments */
class CommentList : public Wt::WContainerWidget {
public:
    /** The type of commenting */
    enum CommentType {
        CHAT, /**< One line messages */
        FORUM /**< Large texts */
    };

    /** Constructor */
    CommentList(const CommentPtr& root, CommentType type,
                Wt::WContainerWidget* parent = 0);

private:
    CommentType type_;
};

}

#endif

