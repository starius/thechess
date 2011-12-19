/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/IntervalWidget.hpp"

namespace thechess {

IntervalWidget:: IntervalWidget(Wt::WWidget* min, Wt::WWidget* max,
                                Wt::WContainerWidget* parent):
    Wt::WTemplate(tr("tc.common.interval_template"), parent) {
    bindWidget("min", min);
    bindWidget("max", max);
}

}

