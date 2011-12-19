/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_CLASSIFICATION_WIDGET_HPP
#define THECHESS_WIDGETS_CLASSIFICATION_WIDGET_HPP

#include <Wt/WComboBox>

#include "model/global.hpp"

namespace thechess {

class ClassificationWidget : public Wt::WComboBox {
public:
    ClassificationWidget(Classification min = NO_CLASSIFICATION,
                         Classification value = NO_CLASSIFICATION,
                         Classification max = SUPER_GRANDMASTER,
                         Wt::WContainerWidget* parent = 0);
    Classification value() const;

private:
    int first_i_;
};

}

#endif

