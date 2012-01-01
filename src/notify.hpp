/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Wc/Notify.hpp>

#include "Application.hpp"
#include "model/object.hpp"

#ifndef THECHESS_NOTIFY_HPP_
#define THECHESS_NOTIFY_HPP_

#define NOTIFIER &tApp->server().notifier()

namespace thechess {

typedef Wt::Wc::notify::Server<Object> Notifier;
typedef Wt::Wc::notify::Widget<Object> Notifiable;

}

#endif

