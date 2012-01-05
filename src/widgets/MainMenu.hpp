/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MAINMENU_HPP_
#define THECHESS_WIDGETS_MAINMENU_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>

#include "thechess-global.hpp"

namespace thechess {

/** Main menu with references */
class MainMenu : public Wt::WCompositeWidget {
public:
    /** Constructor */
    MainMenu(Path* path, Wt::WContainerWidget* parent = 0);

    /** Show or hide user-only items */
    void show_user_items(bool show = true);

private:
    class MainMenuImpl;

    MainMenuImpl* impl_;
};

}

#endif

