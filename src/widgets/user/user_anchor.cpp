/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WAnchor>
#include <Wt/WText>

#include "widgets/user/user_anchor.hpp"
#include "widgets/user/Gravatar.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "Path.hpp"

namespace thechess {

Wt::WWidget* user_anchor(const UserPtr& user, Wt::WContainerWidget* parent) {
    dbo::Transaction t(tApp->session());
    Wt::WContainerWidget* result = new Wt::WContainerWidget(parent);
    result->setInline(true);
    Gravatar* gravatar = new Gravatar(user, result);
    gravatar->set_size(15);
    result->addWidget(gravatar);
    Wt::WAnchor* anchor = new Wt::WAnchor(result);
    anchor->setLink(tApp->path().user_view()->get_link(user.id()));
    anchor->setText(user->safe_username());
    if (user->online()) {
        result->addWidget(new Wt::WText(Wt::WString::tr("tc.user.Online")));
    }
    return result;
}

}

