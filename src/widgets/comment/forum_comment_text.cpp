/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Utils>

#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

Wt::WString forum_comment_text(const CommentPtr& comment) {
    Wt::WString text = comment->text_or_removed(tApp->user());
    if (!Wt::Utils::removeScript(text)) {
        text = Wt::Utils::htmlEncode(text);
    }
    Wt::WString username = comment->init() ?
                           comment->init()->username() :
                           Wt::WString::tr("tc.user.Anonymous");
    return Wt::WString::tr("tc.forum.comment_template")
           .arg(comment->created().toString())
           .arg(username)
           .arg(text);
}

}

