/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_RATING_CHANGES_HPP_
#define THECHESS_WIDGETS_RATING_CHANGES_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

#include "model/all.hpp"

namespace thechess {

class RatingChangesImpl;

class RatingChanges : public Wt::WCompositeWidget {
public:
    RatingChanges(const UserPtr& user, Wt::WContainerWidget* parent = 0);
    void add_user(const UserPtr& user);

private:
    RatingChangesImpl* impl_;
};

}

#endif

