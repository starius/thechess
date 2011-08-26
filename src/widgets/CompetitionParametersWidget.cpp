/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WPushButton>
#include <Wt/WEnvironment>
#include <Wt/WApplication>

#include "widgets/CompetitionParametersWidget.hpp"
#include "widgets/TimeDeltaWidget.hpp"
#include "widgets/ClassificationWidget.hpp"
#include "widgets/MySpinBox.hpp"
#include "widgets/IntervalWidget.hpp"
#include "config.hpp"

namespace thechess {
using namespace config::competition; // min, max

CompetitionParametersWidget::CompetitionParametersWidget(
    const CompetitionParameters* cp,
    bool allow_change_type, Wt::WContainerWidget* parent):
    GameParametersWidget(cp, parent) {
    IntervalWidget* interval;
    Wt::WContainerWidget* cell;
    section(tr("tc.competition.Parameters"));
    type_ = new Wt::WComboBox();
    type_->addItem(Competition::type2str(CLASSICAL));
    type_->addItem(Competition::type2str(STAGED));
    type_->addItem(Competition::type2str(TEAM));
    cell = item(tr("tc.competition.Type"), "", type_, type_);
    set_type_(cp->type());
    if (allow_change_type) {
        if (wApp->environment().ajax()) {
            type_->activated().connect(this,
                                       &CompetitionParametersWidget::type_handler_);
        } else {
            Wt::WPushButton* refresh =
                new Wt::WPushButton(tr("tc.common.Refresh"), cell);
            refresh->clicked().connect(this,
                                       &CompetitionParametersWidget::type_handler_);
        }
    } else {
        type_->disable();
    }
    min_rating_ = new MySpinBox();
    min_rating_->setRange(min::min_rating, max::min_rating);
    min_rating_->setValue(cp->min_rating());
    max_rating_ = new MySpinBox();
    max_rating_->setRange(min::max_rating, max::max_rating);
    max_rating_->setValue(cp->max_rating());
    interval = new IntervalWidget(min_rating_, max_rating_);
    item(tr("tc.competition.Rating"), "", min_rating_, interval);
    min_classification_ = new ClassificationWidget(min::min_classification,
            cp->min_classification(), max::min_classification);
    max_classification_ = new ClassificationWidget(min::max_classification,
            cp->max_classification(), max::max_classification);
    interval = new IntervalWidget(min_classification_, max_classification_);
    item(tr("tc.competition.Members_classification"), "", min_classification_, interval);
    force_start_delay_ = new TimeDeltaWidget(min::force_start_delay,
            cp->force_start_delay(), max::force_start_delay);
    item(tr("tc.competition.Force_start_delay"), "",
         force_start_delay_->form_widget(), force_start_delay_);
    min_users_ = new MySpinBox();
    min_users_->setRange(min::min_users, max::min_users);
    min_users_->setValue(cp->min_users());
    max_users_ = new MySpinBox();
    max_users_->setRange(min::max_users, max::max_users);
    max_users_->setValue(cp->max_users());
    users_ = new IntervalWidget(min_users_, max_users_);
    item(tr("tc.competition.Users"), "", min_users_, users_);
    min_recruiting_time_ = new TimeDeltaWidget(min::min_recruiting_time,
            cp->min_recruiting_time(), max::min_recruiting_time);
    max_recruiting_time_ = new TimeDeltaWidget(min::max_recruiting_time,
            cp->max_recruiting_time(), max::max_recruiting_time);
    recruiting_time_ = new IntervalWidget(min_recruiting_time_, max_recruiting_time_);
    item(tr("tc.competition.Recruiting_time"), "",
         min_recruiting_time_->form_widget(), recruiting_time_);
    max_simultaneous_games_ = new MySpinBox();
    max_simultaneous_games_->setRange(min::max_simultaneous_games, max::max_simultaneous_games);
    max_simultaneous_games_->setValue(cp->max_simultaneous_games());
    item(tr("tc.competition.Max_simultaneous_games"), "",
         max_simultaneous_games_, max_simultaneous_games_);
    games_factor_ = new MyDoubleSpinBox();
    games_factor_->setRange(min::games_factor, max::games_factor);
    games_factor_->setValue(cp->games_factor());
    item(tr("tc.competition.Games_factor"), "", games_factor_, games_factor_);
    relax_time_ = new TimeDeltaWidget(min::relax_time,
                                      cp->relax_time(), max::relax_time);
    item(tr("tc.competition.Relax_time"), "",
         relax_time_->form_widget(), relax_time_);
    min_substages_ = new MySpinBox();
    min_substages_->setRange(min::min_substages, max::min_substages);
    min_substages_->setValue(cp->min_substages());
    item(tr("tc.competition.Min_substages"), "",
         min_substages_, min_substages_);
    increment_substages_ = new MySpinBox();
    increment_substages_->setRange(min::increment_substages, max::increment_substages);
    increment_substages_->setValue(cp->increment_substages());
    item(tr("tc.competition.Increment_substages"), "",
         increment_substages_, increment_substages_);
    type_handler_();
}

void CompetitionParametersWidget::apply_parameters(CompetitionParameters* cp) {
    GameParametersWidget::apply_parameters(cp);
    CompetitionType t = get_type_();
    cp->set_type(t);
    cp->set_min_rating(min_rating_->value());
    cp->set_max_rating(max_rating_->value());
    cp->set_min_classification(min_classification_->value());
    cp->set_max_classification(max_classification_->value());
    cp->set_force_start_delay(force_start_delay_->value());
    if (t == CLASSICAL || t == STAGED) {
        cp->set_min_users(min_users_->value());
        cp->set_max_users(max_users_->value());
        cp->set_min_recruiting_time(min_recruiting_time_->value());
        cp->set_max_recruiting_time(max_recruiting_time_->value());
    }
    if (t == CLASSICAL) {
        cp->set_max_simultaneous_games(max_simultaneous_games_->value());
        cp->set_games_factor(games_factor_->value());
    }
    if (t == STAGED) {
        cp->set_relax_time(relax_time_->value());
        cp->set_min_substages(min_substages_->value());
        cp->set_increment_substages(increment_substages_->value());
    }
    if (cp->min_rating() > cp->max_rating()) {
        cp->set_max_rating(cp->min_rating());
    }
    if (cp->min_classification() > cp->max_classification()) {
        cp->set_max_classification(cp->min_classification());
    }
    if (t == CLASSICAL || t == STAGED) {
        if (cp->min_users() > cp->max_users()) {
            cp->set_min_users(cp->max_users());
        }
        if (cp->min_recruiting_time() > cp->max_recruiting_time()) {
            cp->set_min_recruiting_time(cp->max_recruiting_time());
        }
    }
}

void CompetitionParametersWidget::type_handler_() {
    CompetitionType t = get_type_();
    hide(users_);
    hide(recruiting_time_);
    hide(max_simultaneous_games_);
    hide(games_factor_);
    hide(relax_time_);
    hide(min_substages_);
    hide(increment_substages_);
    if (t == CLASSICAL || t == STAGED) {
        show(users_);
        show(recruiting_time_);
    }
    if (t == CLASSICAL) {
        show(max_simultaneous_games_);
        show(games_factor_);
    }
    if (t == STAGED) {
        show(relax_time_);
        show(min_substages_);
        show(increment_substages_);
    }
}

void CompetitionParametersWidget::set_type_(CompetitionType t) {
    if (t == CLASSICAL) {
        type_->setCurrentIndex(0);
    } else if (t == STAGED) {
        type_->setCurrentIndex(1);
    } else if (t == TEAM) {
        type_->setCurrentIndex(2);
    }
}

CompetitionType CompetitionParametersWidget::get_type_() const {
    CompetitionType t = defaults::competition_type;
    int index = type_->currentIndex();
    if (index == 0) {
        t = CLASSICAL;
    } else if (index == 1) {
        t = STAGED;
    } else if (index == 2) {
        t = TEAM;
    }
    return t;
}

CompetitionParametersWidget2::CompetitionParametersWidget2(const Competition* c,
        bool allow_change_type, Wt::WContainerWidget* parent):
    CompetitionParametersWidget(c, allow_change_type, parent) {
    name_ = new Wt::WLineEdit(c->name());
    name_->setMaxLength(config::competition::max_name);
    item(tr("tc.competition.Name"), "", name_, name_);
    description_ = new Wt::WTextEdit(c->description());
    item(tr("tc.competition.Description"), "", description_, description_);
}

void CompetitionParametersWidget2::apply_parameters(Competition* c) {
    CompetitionParametersWidget::apply_parameters(c);
    std::wstring name = name_->text().value();
    if (name.length() > config::competition::max_name) {
        name.resize(config::competition::max_name);
    }
    c->set_name(name);
    std::wstring description = description_->text().value();
    if (description.length() > config::competition::max_description) {
        description.resize(config::competition::max_description);
    }
    c->set_description(description);
}

}

