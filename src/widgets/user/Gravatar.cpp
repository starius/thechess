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
    if (!user->avatar_path().empty()) {
        set_default(user->avatar_path());
    } else {
        set_default(Wt::Wc::GravatarImage::WAVATAR);
    }
    set_force_default(!user->has_permission(AVATAR_NONDEFAULT));
}

}

