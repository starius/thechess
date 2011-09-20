/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MYGAMESLIST_HPP_
#define THECHESS_WIDGETS_MYGAMESLIST_HPP_

#include <Wt/WCompositeWidget>

#include "model/all.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {

class MyGamesListImpl;

class MyGamesList : public Wt::WCompositeWidget {
public:
    MyGamesList(const UserPtr& user, Wt::WContainerWidget* p = 0);
};

}

#endif

