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

class UserAnchor : public Wt::WContainerWidget, public Notifiable {
public:
    UserAnchor(const UserPtr& user, Wt::WContainerWidget* parent):
        Wt::WContainerWidget(parent),
        Notifiable(Object(USER, user.id())),
        user_(user) {
        setInline(true);
        reprint();
    }

    void notify(EventPtr) {
        dbo::Transaction t(tApp->session());
        user_.reread();
        reprint();
    }

    void reprint() {
        dbo::Transaction t(tApp->session());
        clear();
        Gravatar* gravatar = new Gravatar(user_, this);
        gravatar->set_size(15);
        addWidget(gravatar);
        Wt::WAnchor* anchor = new Wt::WAnchor(this);
        anchor->setLink(tApp->path().user_view()->get_link(user_.id()));
        anchor->setText(user_->safe_username());
        if (user_->online()) {
            addWidget(new Wt::WText(Wt::WString::tr("tc.user.Online")));
        }
    }

private:
    UserPtr user_;
};

Wt::WWidget* user_anchor(const UserPtr& user, Wt::WContainerWidget* parent) {
    if (user) {
        return new UserAnchor(user, parent);
    } else {
        return new Wt::WText();
    }
}

}

