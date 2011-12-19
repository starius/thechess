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

#include "widgets/GPWidget.hpp"
#include "model/all.hpp"

namespace thechess {

class MySpinBox;
class MyDoubleSpinBox;
class TimeDeltaWidget;
class ClassificationWidget;
class IntervalWidget;

class CPWidget : public GPWidget {
public:
    CPWidget(const CP* cp,
             bool allow_change_type = true, Wt::WContainerWidget* parent = 0);

    void apply_parameters(CP* cp);

private:
    Wt::WComboBox* type_;
    MySpinBox* min_rating_;
    MySpinBox* max_rating_;
    ClassificationWidget* min_classification_;
    ClassificationWidget* max_classification_;

    TimeDeltaWidget* force_start_delay_;

    MySpinBox* min_users_;
    MySpinBox* max_users_;
    IntervalWidget* users_;
    TimeDeltaWidget* min_recruiting_time_;
    TimeDeltaWidget* max_recruiting_time_;
    IntervalWidget* recruiting_time_;

    MySpinBox* max_simultaneous_games_;
    MyDoubleSpinBox* games_factor_;

    TimeDeltaWidget* relax_time_;
    MySpinBox* min_substages_;
    MySpinBox* increment_substages_;

    void type_handler_();
    CompetitionType get_type_() const;
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

