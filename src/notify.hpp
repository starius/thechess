/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Wc/Notify.hpp>

#include "Application.hpp"

#ifndef THECHESS_NOTIFY_HPP_
#define THECHESS_NOTIFY_HPP_

#define tNot (&tApp->server().notifier())

namespace thechess {

/** Notifier class */
typedef Wt::Wc::notify::Server Notifier;

/** Notifiable widget */
typedef Wt::Wc::notify::Widget Notifiable;

/** Event for notifications */
typedef Wt::Wc::notify::Event Event;

/** Shared pointer to Event */
typedef Wt::Wc::notify::EventPtr EventPtr;

}

#endif
