/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_

#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

class CompetitionParametersWidget2;

class CompetitionCreateWidget : public Wt::WContainerWidget {
public:
    CompetitionCreateWidget(Wt::WContainerWidget* p = 0);
    CompetitionCreateWidget(CompetitionPtr c, Wt::WContainerWidget* p = 0);
    Wt::EventSignal<Wt::WMouseEvent>& saved();

private:
    CompetitionParametersWidget2* cpw_;
    CompetitionPtr c_;
    Wt::WPushButton* ok_;

    void button_handler_();
};

}

#endif

