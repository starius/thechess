/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MAINMENU_HPP_
#define THECHESS_WIDGETS_MAINMENU_HPP_

#include <Wt/WCompositeWidget>

namespace Wt {
class WContainerWidget;
}

namespace thechess {
namespace widgets {

class MainMenuImpl;

class MainMenu : public Wt::WCompositeWidget {
public:
    MainMenu(Wt::WContainerWidget* parent = 0);
private:
    MainMenuImpl* impl_;
};

}
}

#endif
