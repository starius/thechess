/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_PARAMETERS_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_PARAMETERS_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/Wc/global.hpp>

#include "widgets/RecordWidgetBase.hpp"
#include "widgets/global.hpp"
#include "model/all.hpp"

namespace thechess {

/** Competition parameters editor/creator */
class CPWidget : public Wt::WContainerWidget, RecordWidgetBase {
public:
    /** Constructor.
    \param cp      Source of parameters (0 means the default parameters).
    \param allow_change_type Whether the type of competition may be changed.
    \param parent  Parent widget.
    */
    CPWidget(const CP* cp,
             bool allow_change_type = true, Wt::WContainerWidget* parent = 0);

    /** Apply parameters */
    void apply_parameters(CP* cp);

private:
    Wt::Wc::TableForm* form_;
    GPSelector* gp_selector_;

    CompetitionTypeWidget* type_;
    Wt::Wc::ConstrainedSpinBox* min_rating_;
    Wt::Wc::ConstrainedSpinBox* max_rating_;
    ClassificationWidget* min_classification_;
    ClassificationWidget* max_classification_;

    Wt::Wc::TimeDurationWidget* force_start_delay_;

    Wt::Wc::ConstrainedSpinBox* min_users_;
    Wt::Wc::ConstrainedSpinBox* max_users_;
    Wt::Wc::IntervalWidget* users_;
    Wt::Wc::TimeDurationWidget* min_recruiting_time_;
    Wt::Wc::TimeDurationWidget* max_recruiting_time_;
    Wt::Wc::IntervalWidget* recruiting_time_;

    Wt::Wc::ConstrainedSpinBox* max_simultaneous_games_;
    Wt::Wc::ConstrainedDoubleSpinBox* games_factor_;

    Wt::Wc::TimeDurationWidget* relax_time_;
    Wt::Wc::ConstrainedSpinBox* min_substages_;
    Wt::Wc::ConstrainedSpinBox* increment_substages_;

    Wt::Wc::TimeDurationWidget* min_online_time_;
    Wt::Wc::TimeDurationWidget* max_online_time_;

    void type_handler();
    CompetitionType get_type() const;
    void set_type(CompetitionType t);
};

}

#endif

