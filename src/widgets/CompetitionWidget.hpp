/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

#include "model/Competition.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
namespace widgets {

class CompetitionWidget : public Wt::WTemplate, public Notifiable {
public:
    CompetitionWidget(model::CompetitionPtr competition,
                      Wt::WContainerWidget* p=0);

    virtual void notify();

private:
    model::CompetitionPtr c;

    void reprint_();
};

}
}

#endif

