/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_USERANCHOR_HPP_
#define THECHESS_WIDGETS_USERANCHOR_HPP_

#include <Wt/WGlobal>

#include "model/global.hpp"

namespace thechess {

/** Return widget, containing anchor to user's page */
Wt::WWidget* user_anchor(const UserPtr& user, Wt::WContainerWidget* parent = 0);

}

#endif

