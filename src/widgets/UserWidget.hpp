/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_USEREWIDGET_HPP_
#define THECHESS_WIDGETS_USEREWIDGET_HPP_

#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include "model/User.hpp"

namespace Wt {
class WContainerWidget;
}

namespace thechess {
namespace widgets {

using model::UserPtr;

class UserWidgetImpl;

class UserWidget : public Wt::WCompositeWidget {
public:
    UserWidget(UserPtr user, Wt::WContainerWidget* parent=0);
private:
    UserWidgetImpl* impl_;
};

}
}

#endif
