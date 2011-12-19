/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/ClassificationWidget.hpp"
#include "model/all.hpp"

namespace thechess {

ClassificationWidget::ClassificationWidget(Classification min,
        Classification value, Classification max, Wt::WContainerWidget* parent):
    Wt::WComboBox(parent), first_i_(-1) {
    for (int i = 0; i < CLASSIFICATIONS_SIZE; i++) {
        Classification classification = CLASSIFICATIONS[i];
        if (min <= classification && classification <= max) {
            addItem(User::classification2str(classification));
            if (classification == value) {
                setCurrentIndex(i);
            }
            if (first_i_ == -1) {
                first_i_ = i;
            }
        }
    }
}

Classification ClassificationWidget::value() const {
    return CLASSIFICATIONS[currentIndex() + first_i_];
}

}

