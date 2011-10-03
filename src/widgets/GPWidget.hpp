/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_
#define THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WCheckBox>

#include "model/all.hpp"
#include "utils/TableForm.hpp"

namespace thechess {

class MySpinBox;
class TimeDeltaWidget;
class MovesWidget;

class GPWidget : public Wt::TableForm {
public:
    GPWidget(const GP* gp,
             Wt::WContainerWidget* parent = 0);
    void apply_parameters(GP* gp);

private:
    MovesWidget* moves_widget_;

    TimeDeltaWidget* limit_std_;
    TimeDeltaWidget* limit_private_init_;

    Wt::WCheckBox* norating_;

    TimeDeltaWidget* pause_limit_init_;
    MySpinBox* first_draw_;
};

}

#endif

