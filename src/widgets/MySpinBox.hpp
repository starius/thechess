/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MYSPINBOX_HPP_
#define THECHESS_WIDGETS_MYSPINBOX_HPP_

#include <Wt/WSpinBox>
#include <Wt/WDoubleSpinBox>

namespace Wt {
class WContainerWidget;
}

namespace thechess {
namespace widgets {

class MySpinBox : public Wt::WSpinBox {
public:
    MySpinBox(Wt::WContainerWidget* parent=0);
    int value() const;
};

class MyDoubleSpinBox : public Wt::WDoubleSpinBox {
public:
    MyDoubleSpinBox(Wt::WContainerWidget* parent=0);
    double value() const;
};

}
}

#endif

