/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_FORUM_COMMENT_TEXT_
#define THECHESS_FORUM_COMMENT_TEXT_

#include <Wt/WString>

#include "model/global.hpp"

namespace thechess {

Wt::WString forum_comment_text(const CommentPtr& comment);

}

#endif

