/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_RATING_CHANGES_HPP_
#define THECHESS_WIDGETS_RATING_CHANGES_HPP_

#include <Wt/WCompositeWidget>

#include "model/User.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {

class RatingChangesImpl;

class RatingChanges : public Wt::WCompositeWidget {
public:
    RatingChanges(UserPtr user, Wt::WContainerWidget* parent = 0);
    void add_user(UserPtr user);

private:
    RatingChangesImpl* impl_;
};

}

#endif

