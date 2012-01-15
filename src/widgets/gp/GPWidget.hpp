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
#include <Wt/Wc/global.hpp>
#include <Wt/Wc/TableForm.hpp>

#include "widgets/global.hpp"
#include "model/all.hpp"

namespace thechess {

/** Game parameters editor/creator */
class GPWidget : public Wt::Wc::TableForm {
public:
    /** Constructor.
    \param gp     Source of parameters (0 means the default parameters).
    \param parent Parent widget.
    */
    GPWidget(const GP* gp,
             Wt::WContainerWidget* parent = 0);

    /** Apply parameters */
    void apply_parameters(GP* gp);

private:
    MovesWidget* moves_widget_;

    Wt::Wc::TimeDurationWidget* limit_std_;
    Wt::Wc::TimeDurationWidget* limit_private_init_;

    Wt::WCheckBox* norating_;

    Wt::Wc::TimeDurationWidget* pause_limit_init_;
    Wt::Wc::ConstrainedSpinBox* first_draw_;
};

}

#endif

