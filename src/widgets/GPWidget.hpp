/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_
#define THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WCheckBox>
#include <Wt/Wc/TableForm.hpp>

#include "model/all.hpp"

namespace thechess {

class MySpinBox;
class TimeDeltaWidget;
class MovesWidget;

class GPWidget : public Wt::Wc::TableForm {
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

