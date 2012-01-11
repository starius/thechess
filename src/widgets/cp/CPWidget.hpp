/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_PARAMETERS_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_PARAMETERS_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WCheckBox>
#include <Wt/WTextEdit>
#include <Wt/Wc/global.hpp>

#include "widgets/gp/GPWidget.hpp"
#include "widgets/global.hpp"
#include "model/all.hpp"

namespace thechess {

class CPWidget : public GPWidget {
public:
    CPWidget(const CP* cp,
             bool allow_change_type = true, Wt::WContainerWidget* parent = 0);

    void apply_parameters(CP* cp);

private:
    Wt::WComboBox* type_;
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

    void type_handler();
    CompetitionType get_type() const;
    void set_type_(CompetitionType t);
};

class CPWidget2 : public CPWidget {
public:
    CPWidget2(const Competition* c,
              bool allow_change_type = true, Wt::WContainerWidget* parent = 0);

    void apply_parameters(Competition* c);

private:
    Wt::WLineEdit* name_;
    Wt::WTextEdit* description_;
};

}

#endif

