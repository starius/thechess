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

#include "widgets/CPWidget.hpp"
#include "widgets/TimeDeltaWidget.hpp"
#include "widgets/ClassificationWidget.hpp"
#include "widgets/MySpinBox.hpp"
#include "widgets/IntervalWidget.hpp"
#include "config.hpp"

namespace thechess {

CPWidget::CPWidget(
    const CP* cp,
    bool allow_change_type, Wt::WContainerWidget* parent):
    GPWidget(&(*(cp->gp())), parent) { // FIXME
    using namespace config::competition; // min, max
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
                                       &CPWidget::type_handler_);
        } else {
            Wt::WPushButton* refresh =
                new Wt::WPushButton(tr("tc.common.Refresh"), cell);
            refresh->clicked().connect(this,
                                       &CPWidget::type_handler_);
        }
    } else {
        type_->disable();
    }
    min_rating_ = new MySpinBox();
    min_rating_->setRange(min::MIN_RATING, max::MIN_RATING);
    min_rating_->setValue(cp->min_rating());
    max_rating_ = new MySpinBox();
    max_rating_->setRange(min::MAX_RATING, max::MAX_RATING);
    max_rating_->setValue(cp->max_rating());
    interval = new IntervalWidget(min_rating_, max_rating_);
    item(tr("tc.competition.Rating"), "", min_rating_, interval);
    min_classification_ = new ClassificationWidget(min::MIN_CLASSIFICATION,
            cp->min_classification(), max::MIN_CLASSIFICATION);
    max_classification_ = new ClassificationWidget(min::MAX_CLASSIFICATION,
            cp->max_classification(), max::MAX_CLASSIFICATION);
    interval = new IntervalWidget(min_classification_, max_classification_);
    item(tr("tc.competition.Members_classification"), "", min_classification_, interval);
    force_start_delay_ = new TimeDeltaWidget(min::FORCE_START_DELAY,
            cp->force_start_delay(), max::FORCE_START_DELAY);
    item(tr("tc.competition.Force_start_delay"), "",
         force_start_delay_->form_widget(), force_start_delay_);
    min_users_ = new MySpinBox();
    min_users_->setRange(min::MIN_USERS, max::MIN_USERS);
    min_users_->setValue(cp->min_users());
    max_users_ = new MySpinBox();
    max_users_->setRange(min::MAX_USERS, max::MAX_USERS);
    max_users_->setValue(cp->max_users());
    users_ = new IntervalWidget(min_users_, max_users_);
    item(tr("tc.competition.Users"), "", min_users_, users_);
    min_recruiting_time_ = new TimeDeltaWidget(min::MIN_RECRUITING_TIME,
            cp->min_recruiting_time(), max::MIN_RECRUITING_TIME);
    max_recruiting_time_ = new TimeDeltaWidget(min::MAX_RECRUITING_TIME,
            cp->max_recruiting_time(), max::MAX_RECRUITING_TIME);
    recruiting_time_ = new IntervalWidget(min_recruiting_time_, max_recruiting_time_);
    item(tr("tc.competition.Recruiting_time"), "",
         min_recruiting_time_->form_widget(), recruiting_time_);
    max_simultaneous_games_ = new MySpinBox();
    max_simultaneous_games_->setRange(min::MAX_SIMULTANEOUS_GAMES, max::MAX_SIMULTANEOUS_GAMES);
    max_simultaneous_games_->setValue(cp->max_simultaneous_games());
    item(tr("tc.competition.Max_simultaneous_games"), "",
         max_simultaneous_games_, max_simultaneous_games_);
    games_factor_ = new MyDoubleSpinBox();
    games_factor_->setRange(min::GAMES_FACTOR, max::GAMES_FACTOR);
    games_factor_->setValue(cp->games_factor());
    item(tr("tc.competition.Games_factor"), "", games_factor_, games_factor_);
    relax_time_ = new TimeDeltaWidget(min::RELAX_TIME,
                                      cp->relax_time(), max::RELAX_TIME);
    item(tr("tc.competition.Relax_time"), "",
         relax_time_->form_widget(), relax_time_);
    min_substages_ = new MySpinBox();
    min_substages_->setRange(min::MIN_SUBSTAGES, max::MIN_SUBSTAGES);
    min_substages_->setValue(cp->min_substages());
    item(tr("tc.competition.Min_substages"), "",
         min_substages_, min_substages_);
    increment_substages_ = new MySpinBox();
    increment_substages_->setRange(min::INCREMENT_SUBSTAGES, max::INCREMENT_SUBSTAGES);
    increment_substages_->setValue(cp->increment_substages());
    item(tr("tc.competition.Increment_substages"), "",
         increment_substages_, increment_substages_);
    type_handler_();
}

void CPWidget::apply_parameters(CP* cp) {
    GPWidget::apply_parameters(cp->gp().modify());
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

void CPWidget::type_handler_() {
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

void CPWidget::set_type_(CompetitionType t) {
    if (t == CLASSICAL) {
        type_->setCurrentIndex(0);
    } else if (t == STAGED) {
        type_->setCurrentIndex(1);
    } else if (t == TEAM) {
        type_->setCurrentIndex(2);
    }
}

CompetitionType CPWidget::get_type_() const {
    using namespace config::competition; // defaults
    CompetitionType t = defaults::COMPETITION_TYPE;
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

CPWidget2::CPWidget2(const Competition* c,
                     bool allow_change_type, Wt::WContainerWidget* parent):
    CPWidget(&(*(c->cp())), allow_change_type, parent) { //FIXME
    name_ = new Wt::WLineEdit(c->name());
    name_->setMaxLength(config::competition::MAX_NAME);
    item(tr("tc.competition.Name"), "", name_, name_);
    description_ = new Wt::WTextEdit(c->description());
    item(tr("tc.competition.Description"), "", description_, description_);
}

void CPWidget2::apply_parameters(Competition* c) {
    CPWidget::apply_parameters(c->cp().modify());
    std::wstring name = name_->text().value();
    if (name.length() > config::competition::MAX_NAME) {
        name.resize(config::competition::MAX_NAME);
    }
    c->set_name(name);
    std::wstring description = description_->text().value();
    if (description.length() > config::competition::MAX_DESCRIPTION) {
        description.resize(config::competition::MAX_DESCRIPTION);
    }
    c->set_description(description);
}

}

