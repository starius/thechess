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

namespace thechess {

class MainMenuImpl;

class MainMenu : public Wt::WCompositeWidget {
public:
    MainMenu(Wt::WContainerWidget* parent = 0);
private:
    MainMenuImpl* impl_;
};

}

#endif

