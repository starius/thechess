/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>

#include "widgets/MySpinBox.hpp"

namespace thechess {

MySpinBox::MySpinBox(Wt::WContainerWidget* parent) :
    Wt::WSpinBox(parent) {
}

int MySpinBox::value() const {
    int v = Wt::WSpinBox::value();
    if (v < minimum()) {
        v = minimum();
    } else if (v > maximum()) {
        v = maximum();
    }
    return v;
}

MyDoubleSpinBox::MyDoubleSpinBox(Wt::WContainerWidget* parent) :
    Wt::WDoubleSpinBox(parent) {
}

double MyDoubleSpinBox::value() const {
    double v = Wt::WDoubleSpinBox::value();
    if (v < minimum()) {
        v = minimum();
    } else if (v > maximum()) {
        v = maximum();
    }
    return v;
}

}

