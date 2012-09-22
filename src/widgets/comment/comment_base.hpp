/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMMENT_BASE_HPP_
#define THECHESS_WIDGETS_COMMENT_BASE_HPP_

#include <Wt/WGlobal>

#include "model/object.hpp"

namespace thechess {

/** Return object which is a base for this CHAT_ROOT.
If not found, then result.id == 0.
*/
Object comment_base(const CommentPtr& comment);

/** Return anchor to page of base of this CHAT_ROOT.
If not found, then result == 0.
*/
Wt::WAnchor* anchor_to_host(const CommentPtr& comment);

}

#endif

