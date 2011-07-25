/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_PARAMETERS_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_PARAMETERS_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#include <Wt/WCheckBox>
#include <Wt/WTextEdit>

#include "widgets/GameParametersWidget.hpp"
#include "model/CompetitionParameters.hpp"
#include "model/Competition.hpp"

namespace thechess {
namespace widgets {

class MySpinBox;
class MyDoubleSpinBox;
class TimeDeltaWidget;
class ClassificationWidget;
class IntervalWidget;

class CompetitionParametersWidget : public GameParametersWidget {
public:
    CompetitionParametersWidget(const model::CompetitionParameters* cp,
                                bool allow_change_type=true, Wt::WContainerWidget* parent=0);

    void apply_parameters(model::CompetitionParameters* cp);

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
    model::CompetitionType get_type_() const;
    void set_type_(model::CompetitionType t);
};

class CompetitionParametersWidget2 : public CompetitionParametersWidget {
public:
    CompetitionParametersWidget2(const model::Competition* c,
                                 bool allow_change_type=true, Wt::WContainerWidget* parent=0);

    void apply_parameters(model::Competition* c);

private:
    Wt::WLineEdit* name_;
    Wt::WTextEdit* description_;
};

}
}

#endif

