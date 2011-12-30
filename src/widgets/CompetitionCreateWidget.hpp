/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_

#include <Wt/WContainerWidget>

#include "widgets/global.hpp"
#include "model/all.hpp"

namespace thechess {

class CompetitionCreateWidget : public Wt::WContainerWidget {
public:
    CompetitionCreateWidget(Wt::WContainerWidget* p = 0);
    CompetitionCreateWidget(const CompetitionPtr& c, Wt::WContainerWidget* p = 0);
    Wt::EventSignal<Wt::WMouseEvent>& saved();

private:
    CPWidget2* cpw_;
    CompetitionPtr c_;
    Wt::WPushButton* ok_;

    void button_handler_();
};

}

#endif

