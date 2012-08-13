/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_COMMENT_REMOVER_BUTTONS_HPP_
#define THECHESS_COMMENT_REMOVER_BUTTONS_HPP_

#include <Wt/WGlobal>

#include "model/global.hpp"

namespace thechess {

void add_remover_buttons(const CommentPtr& comment, Wt::WContainerWidget* p);

}

#endif

