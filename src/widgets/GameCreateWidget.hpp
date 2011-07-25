/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMECREATEWIDGET_HPP_
#define THECHESS_WIDGETS_GAMECREATEWIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "model/User.hpp"
#include "chess/field.hpp"

namespace Wt {
class WComboBox;
};

namespace thechess {
namespace widgets {

using model::UserPtr;

class GameParametersWidget;

class GameCreateWidget : public Wt::WContainerWidget {
public:
    GameCreateWidget(UserPtr user, Wt::WContainerWidget* p=0);
    GameCreateWidget(Wt::WContainerWidget* p=0);

private:
    GameParametersWidget* gpw_;
    bool with_user_;
    UserPtr user_;
    Wt::WComboBox* color_;

    void print_();
    void button_handler_();
    chess::Color selected_color_() const;
};

}
}

#endif
