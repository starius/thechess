/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

#include "model/all.hpp"
#include "notify.hpp"

namespace thechess {

class CompetitionWidget : public Wt::WTemplate, public Notifiable {
public:
    CompetitionWidget(const CompetitionPtr& competition,
                      Wt::WContainerWidget* p = 0);

    virtual void notify(EventPtr);

private:
    CompetitionPtr c;

    void reprint_();
};

}

#endif

