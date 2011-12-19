/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_INTERVAL_WIDGET_HPP_
#define THECHESS_WIDGETS_INTERVAL_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

namespace thechess {

class IntervalWidget : public Wt::WTemplate {
public:
    IntervalWidget(Wt::WWidget* min, Wt::WWidget* max,
                   Wt::WContainerWidget* parent = 0);
};

}

#endif

