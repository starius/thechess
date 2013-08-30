/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/cp/CompetitionTypeWidget.hpp"
#include "model/all.hpp"

namespace thechess {

CompetitionTypeWidget::CompetitionTypeWidget(bool with_all,
        Wt::WContainerWidget* p):
    Wt::WComboBox(p),
    with_all_(with_all) {
    if (with_all_) {
        addItem(tr("tc.common.all"));
    }
    addItem(Competition::type2str(CLASSICAL));
    addItem(Competition::type2str(STAGED));
    addItem(Competition::type2str(TEAM));
    addItem(Competition::type2str(PAIR_TEAM));
}

CompetitionType CompetitionTypeWidget::value() const {
    int index = currentIndex();
    if (with_all_) {
        if (index == 0) {
            return NO_COMPETITION_TYPE;
        } else {
            index -= 1;
        }
    }
    if (index == 0) {
        return CLASSICAL;
    } else if (index == 1) {
        return STAGED;
    } else if (index == 2) {
        return TEAM;
    } else if (index == 3) {
        return PAIR_TEAM;
    }
    return NO_COMPETITION_TYPE;
}

void CompetitionTypeWidget::set_value(CompetitionType value) {
    int index = 0;
    if (value == STAGED) {
        index = 0;
    } else if (value == CLASSICAL) {
        index = 1;
    } else if (value == TEAM) {
        index = 2;
    }
    if (with_all_) {
        index += 1;
    }
    setCurrentIndex(index);
}

}

