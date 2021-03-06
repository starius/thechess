/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WPushButton>
#include <Wt/WEnvironment>
#include <Wt/WApplication>
#include <Wt/Wc/TableForm.hpp>
#include <Wt/Wc/ConstrainedSpinBox.hpp>
#include <Wt/Wc/TimeDurationWidget.hpp>
#include <Wt/Wc/IntervalWidget.hpp>

#include "widgets/cp/CPWidget.hpp"
#include "widgets/cp/CompetitionTypeWidget.hpp"
#include "widgets/gp/GPSelector.hpp"
#include "widgets/Header.hpp"
#include "widgets/user/ClassificationWidget.hpp"
#include "config.hpp"

namespace thechess {

CPWidget::CPWidget(const CP* cp, bool allow_change_type,
                   Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent) {
    using namespace config::competition; // min, max
    CP default_cp(true);
    if (!cp) {
        cp = &default_cp;
    }
    new Header(tr("tc.competition.Parameters"), this);
    form_ = new Wt::Wc::TableForm(this);
    Wt::WContainerWidget* cell;
    type_ = new CompetitionTypeWidget(/* with_all */ false);
    cell = form_->item(tr("tc.competition.Type"), "", type_, type_);
    set_type(cp->type());
    if (allow_change_type) {
        if (wApp->environment().ajax()) {
            type_->activated().connect(this,
                                       &CPWidget::type_handler);
        } else {
            Wt::WPushButton* refresh =
                new Wt::WPushButton(tr("tc.common.Refresh"), cell);
            refresh->clicked().connect(this,
                                       &CPWidget::type_handler);
        }
    } else {
        type_->disable();
    }
    min_rating_ = new Wt::Wc::ConstrainedSpinBox();
    min_rating_->setRange(min::MIN_RATING, max::MIN_RATING);
    min_rating_->setValue(cp->min_rating());
    max_rating_ = new Wt::Wc::ConstrainedSpinBox();
    max_rating_->setRange(min::MAX_RATING, max::MAX_RATING);
    max_rating_->setValue(cp->max_rating());
    rating_ = new Wt::Wc::IntervalWidget(min_rating_, max_rating_);
    form_->item(tr("tc.competition.Rating"), "", min_rating_, rating_);
    min_classification_ = new ClassificationWidget(min::MIN_CLASSIFICATION,
            cp->min_classification(), max::MIN_CLASSIFICATION);
    max_classification_ = new ClassificationWidget(min::MAX_CLASSIFICATION,
            cp->max_classification(), max::MAX_CLASSIFICATION);
    classification_ = new Wt::Wc::IntervalWidget(min_classification_,
            max_classification_);
    form_->item(tr("tc.competition.Members_classification"), "",
                min_classification_, classification_);
    //
    min_online_time_ = new Wt::Wc::TimeDurationWidget(
        min::MIN_ONLINE_TIME,
        cp->min_online_time(), max::MIN_ONLINE_TIME);
    max_online_time_ = new Wt::Wc::TimeDurationWidget(
        min::MAX_ONLINE_TIME,
        cp->max_online_time(), max::MAX_ONLINE_TIME);
    online_time_ = new Wt::Wc::IntervalWidget(min_online_time_,
            max_online_time_);
    form_->item(tr("tc.competition.Online_time"), "",
                min_online_time_->form_widget(), online_time_);
    //
    force_start_delay_ = new Wt::Wc::TimeDurationWidget(min::FORCE_START_DELAY,
            cp->force_start_delay(), max::FORCE_START_DELAY);
    form_->item(tr("tc.competition.Force_start_delay"), "",
                force_start_delay_->form_widget(), force_start_delay_);
    min_users_ = new Wt::Wc::ConstrainedSpinBox();
    min_users_->setRange(min::MIN_USERS, max::MIN_USERS);
    min_users_->setValue(cp->min_users());
    max_users_ = new Wt::Wc::ConstrainedSpinBox();
    max_users_->setRange(min::MAX_USERS, max::MAX_USERS);
    max_users_->setValue(cp->max_users());
    users_ = new Wt::Wc::IntervalWidget(min_users_, max_users_);
    form_->item(tr("tc.competition.Users"), "", min_users_, users_);
    min_recruiting_time_ = new Wt::Wc::TimeDurationWidget(
        min::MIN_RECRUITING_TIME,
        cp->min_recruiting_time(), max::MIN_RECRUITING_TIME);
    max_recruiting_time_ = new Wt::Wc::TimeDurationWidget(
        min::MAX_RECRUITING_TIME,
        cp->max_recruiting_time(), max::MAX_RECRUITING_TIME);
    recruiting_time_ = new Wt::Wc::IntervalWidget(min_recruiting_time_,
            max_recruiting_time_);
    form_->item(tr("tc.competition.Recruiting_time"), "",
                min_recruiting_time_->form_widget(), recruiting_time_);
    relax_time_ = new Wt::Wc::TimeDurationWidget(min::RELAX_TIME,
            cp->relax_time(), max::RELAX_TIME);
    form_->item(tr("tc.competition.Relax_time"), "",
                relax_time_->form_widget(), relax_time_);
    max_simultaneous_games_ = new Wt::Wc::ConstrainedSpinBox();
    max_simultaneous_games_->setRange(min::MAX_SIMULTANEOUS_GAMES,
                                      max::MAX_SIMULTANEOUS_GAMES);
    max_simultaneous_games_->setValue(cp->max_simultaneous_games());
    form_->item(tr("tc.competition.Max_simultaneous_games"), "",
                max_simultaneous_games_, max_simultaneous_games_);
    games_factor_ = new Wt::Wc::ConstrainedDoubleSpinBox();
    games_factor_->setRange(min::GAMES_FACTOR, max::GAMES_FACTOR);
    games_factor_->setValue(cp->games_factor());
    form_->item(tr("tc.competition.Games_factor"), "",
                games_factor_, games_factor_);
    min_substages_ = new Wt::Wc::ConstrainedSpinBox();
    min_substages_->setRange(min::MIN_SUBSTAGES, max::MIN_SUBSTAGES);
    min_substages_->setValue(cp->min_substages());
    form_->item(tr("tc.competition.Min_substages"), "",
                min_substages_, min_substages_);
    increment_substages_ = new Wt::Wc::ConstrainedSpinBox();
    increment_substages_->setRange(min::INCREMENT_SUBSTAGES,
                                   max::INCREMENT_SUBSTAGES);
    increment_substages_->setValue(cp->increment_substages());
    form_->item(tr("tc.competition.Increment_substages"), "",
                increment_substages_, increment_substages_);
    add_record_inputs(cp, form_);
    gp_selector_ = new GPSelector(this);
    gp_selector_->select_first();
    gp_selector_->set_gp(cp->gp());
    type_handler();
}

void CPWidget::apply_parameters(CP* cp) {
    cp->set_gp(gp_selector_->gp());
    write_record(cp);
    CompetitionType t = get_type();
    cp->set_type(t);
    if (has_rating(t)) {
        cp->set_min_rating(min_rating_->corrected_value());
        cp->set_max_rating(max_rating_->corrected_value());
        if (cp->min_rating() > cp->max_rating()) {
            cp->set_max_rating(cp->min_rating());
        }
    }
    if (has_classification(t)) {
        cp->set_min_classification(min_classification_->value());
        cp->set_max_classification(max_classification_->value());
    }
    if (has_online_time(t)) {
        cp->set_min_online_time(min_online_time_->value());
        cp->set_max_online_time(max_online_time_->value());
    }
    if (has_force_start_delay(t)) {
        cp->set_force_start_delay(force_start_delay_->corrected_value());
    }
    if (has_users_number(t)) {
        cp->set_min_users(min_users_->corrected_value());
        cp->set_max_users(max_users_->corrected_value());
        if (cp->min_users() > cp->max_users()) {
            cp->set_min_users(cp->max_users());
        }
    }
    if (has_recruiting_time(t)) {
        cp->set_min_recruiting_time(min_recruiting_time_->corrected_value());
        cp->set_max_recruiting_time(max_recruiting_time_->corrected_value());
        if (cp->min_recruiting_time() > cp->max_recruiting_time()) {
            cp->set_min_recruiting_time(cp->max_recruiting_time());
        }
    }
    if (has_max_simultaneous_games(t)) {
        int max_simultaneous_games = max_simultaneous_games_->corrected_value();
        cp->set_max_simultaneous_games(max_simultaneous_games);
    }
    if (has_games_factor(t)) {
        cp->set_games_factor(games_factor_->corrected_value());
    }
    if (has_relax_time(t)) {
        cp->set_relax_time(relax_time_->corrected_value());
    }
    if (has_substages(t)) {
        cp->set_min_substages(min_substages_->corrected_value());
        cp->set_increment_substages(increment_substages_->corrected_value());
    }
    if (cp->min_classification() > cp->max_classification()) {
        cp->set_max_classification(cp->min_classification());
    }
}

void CPWidget::hide_gp_selector() {
    gp_selector_->hide();
}

void CPWidget::type_handler() {
    CompetitionType t = get_type();
    form_->set_visible(rating_, has_rating(t));
    form_->set_visible(classification_, has_classification(t));
    form_->set_visible(force_start_delay_, has_force_start_delay(t));
    form_->set_visible(users_, has_users_number(t));
    form_->set_visible(recruiting_time_, has_recruiting_time(t));
    form_->set_visible(max_simultaneous_games_, has_max_simultaneous_games(t));
    form_->set_visible(games_factor_, has_games_factor(t));
    form_->set_visible(relax_time_, has_relax_time(t));
    form_->set_visible(min_substages_, has_substages(t));
    form_->set_visible(increment_substages_, has_substages(t));
    form_->set_visible(online_time_, has_online_time(t));
}

void CPWidget::set_type(CompetitionType t) {
    type_->set_value(t);
}

CompetitionType CPWidget::get_type() const {
    return type_->value();
}

}

