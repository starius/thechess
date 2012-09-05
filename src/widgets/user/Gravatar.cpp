/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/user/Gravatar.hpp"
#include "Application.hpp"

namespace thechess {

Gravatar::Gravatar(const UserPtr& user, Wt::WContainerWidget* parent):
    GravatarImage("", parent) {
    dbo::Transaction t(tApp->session());
    set_email(user->email());
    set_default(Wt::Wc::GravatarImage::WAVATAR);
}

}

