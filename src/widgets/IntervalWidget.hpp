/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_INTERVAL_WIDGET_HPP_
#define THECHESS_WIDGETS_INTERVAL_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

namespace thechess {
namespace widgets {

class IntervalWidget : public Wt::WTemplate {
public:
    IntervalWidget(Wt::WWidget* min, Wt::WWidget* max,
                   Wt::WContainerWidget* parent=0);
};

}
}

#endif

