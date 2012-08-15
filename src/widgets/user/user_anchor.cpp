/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WAnchor>

#include "widgets/user/user_anchor.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "Path.hpp"

namespace thechess {

Wt::WAnchor* user_anchor(const UserPtr& user, Wt::WContainerWidget* parent) {
    dbo::Transaction t(tApp->session());
    Wt::WAnchor* result = new Wt::WAnchor(parent);
    result->setLink(tApp->path().user_view()->get_link(user.id()));
    result->setText(user->safe_username());
    return result;
}

}

