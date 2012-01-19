/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "model/all.hpp"

namespace thechess {

Comment::Comment()
{ }

Comment::Comment(bool):
    commentable_(true),
    state_(OK),
    created_(now()),
    edited_(now())
{ }

void Comment::set_text(const Wt::WString& text) {
    text_ = text;
    edited_ = now();
}

}

