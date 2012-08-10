/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/Wc/global.hpp>

#include "widgets/global.hpp"
#include "widgets/RecordWidgetBase.hpp"
#include "model/all.hpp"

namespace thechess {

/** Competition creation or modifying */
class CompetitionCreateWidget : public Wt::WContainerWidget, RecordWidgetBase {
public:
    /** Constructor for creation of new competition */
    CompetitionCreateWidget(Wt::WContainerWidget* p = 0);

    /** Constructor for modifying of the competition */
    CompetitionCreateWidget(const CompetitionPtr& c,
                            Wt::WContainerWidget* p = 0);

    /** Signal emmited when a competition is saved */
    Wt::EventSignal<Wt::WMouseEvent>& saved();

private:
    CPSelector* cp_selector_;
    GPSelector* gp_selector_;
    Wt::Wc::TableForm* form_;
    CompetitionPtr c_;
    Wt::WPushButton* ok_;
    Wt::WText* error_message_;

    void print();
    void button_handler();
};

}

#endif

