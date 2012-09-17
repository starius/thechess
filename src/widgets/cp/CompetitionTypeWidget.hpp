/*
 * thechess, chess competition web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_TYPE_HPP_
#define THECHESS_WIDGETS_COMPETITION_TYPE_HPP_

#include <Wt/WGlobal>
#include <Wt/WComboBox>

#include "model/global.hpp"

namespace thechess {

/** Select CompetitionType */
class CompetitionTypeWidget : public Wt::WComboBox {
public:
    /** Constructor */
    CompetitionTypeWidget(bool with_all = true,
                          Wt::WContainerWidget* p = 0);

    /** Get selected competition type */
    CompetitionType value() const;

    /** Set selected competition type */
    void set_value(CompetitionType value);

private:
    bool with_all_;
};

}

#endif

