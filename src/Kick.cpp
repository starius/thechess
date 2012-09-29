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
    Notifiable("kick-" + TO_S(tApp->user().id()))
{ }

static void logout_app() {
    dbo::Transaction t(tApp->session());
    bool removed = true;
    if (tApp->user()) {
        tApp->user().reread();
        if (!tApp->user()->removed()) {
            removed = false;
        }
    }
    if (removed) {
        wApp->redirect("/html/removed.html");
    } else {
        wApp->redirect("/html/kicked.html");
    }
    wApp->triggerUpdate();
}

void Kick::notify(EventPtr) {
    dbo::Transaction t(tApp->session());
    if (tApp->user()) {
        Wt::Wc::bound_post(logout_app)();
    }
}

}

