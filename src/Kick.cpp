/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Wc/util.hpp>

#include "Kick.hpp"
#include "Application.hpp"
#include "Session.hpp"
#include "model/all.hpp"

namespace thechess {

Kick::Kick():
    Notifiable(Object(USER, tApp->user().id()), tNot)
{ }

static void logout_app() {
    tApp->session().login().logout();
    wApp->triggerUpdate();
}

void Kick::notify(EventPtr) {
    if (!tApp->user()) {
        return;
    }
    dbo::Transaction t(tApp->session());
    tApp->user().reread();
    tApp->user()->auth_info().reread();
    if (!tApp->user() || tApp->user()->removed()) {
        Wt::Wc::bound_post(logout_app)();
    }
    t.commit();
}

}

