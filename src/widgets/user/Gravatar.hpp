/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GRAVATAR_HPP_
#define THECHESS_WIDGETS_GRAVATAR_HPP_

#include <Wt/WGlobal>
#include <Wt/Wc/GravatarImage.hpp>

#include "model/all.hpp"

namespace thechess {

/** List of users */
class Gravatar : public Wt::Wc::GravatarImage {
public:
    /** Constructor */
    Gravatar(const UserPtr& user, Wt::WContainerWidget* parent = 0);
};

}

#endif

