
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
namespace widgets {
using namespace model;
using namespace config::competition; // min, max

CompetitionParametersWidget::CompetitionParametersWidget(
    const CompetitionParameters* cp,
    bool allow_change_type, Wt::WContainerWidget* parent):
GameParametersWidget(cp, parent)
{
    IntervalWidget* interval;
    Wt::WContainerWidget* cell;

    section(tr("thechess.competition_parameters"));

    type_ = new Wt::WComboBox();
    type_->addItem(tr("thechess.competition.type.CLASSICAL"));
    type_->addItem(tr("thechess.competition.type.STAGED"));
    type_->addItem(tr("thechess.competition.type.TEAM"));
    cell = item(tr("thechess.competition.type"), "", type_, type_);
    set_type_(cp->type());
    if (allow_change_type)
    {
        if (wApp->environment().ajax())
        {
            type_->activated().connect(this,
                &CompetitionParametersWidget::type_handler_);
        }
        else
        {
            Wt::WPushButton* refresh =
                new Wt::WPushButton(tr("thechess.refresh"), cell);
            refresh->clicked().connect(this,
                &CompetitionParametersWidget::type_handler_);
        }
    }
    else
    {
        type_->disable();
    }

    min_rating_ = new MySpinBox();
    min_rating_->setRange(min::min_rating, max::min_rating);
    min_rating_->setValue(cp->min_rating());
    max_rating_ = new MySpinBox();
    max_rating_->setRange(min::max_rating, max::max_rating);
    max_rating_->setValue(cp->max_rating());
    interval = new IntervalWidget(min_rating_, max_rating_);
    item(tr("thechess.competition.rating"), "", min_rating_, interval);

    min_classification_ = new ClassificationWidget(min::min_classification,
        cp->min_classification(), max::min_classification);
    max_classification_ = new ClassificationWidget(min::max_classification,
        cp->max_classification(), max::max_classification);
    interval = new IntervalWidget(min_classification_, max_classification_);
    item(tr("thechess.competition.classification"), "", min_classification_, interval);

    force_start_delay_ = new TimeDeltaWidget(min::force_start_delay,
        cp->force_start_delay(), max::force_start_delay);
    item(tr("thechess.competition.force_start_delay"), "",
        force_start_delay_->form_widget(), force_start_delay_);

    min_users_ = new MySpinBox();
    min_users_->setRange(min::min_users, max::min_users);
    min_users_->setValue(cp->min_users());
    max_users_ = new MySpinBox();
    max_users_->setRange(min::max_users, max::max_users);
    max_users_->setValue(cp->max_users());
    users_ = new IntervalWidget(min_users_, max_users_);
    item(tr("thechess.competition.users"), "", min_users_, users_);

    min_recruiting_time_ = new TimeDeltaWidget(min::min_recruiting_time,
        cp->min_recruiting_time(), max::min_recruiting_time);
    max_recruiting_time_ = new TimeDeltaWidget(min::max_recruiting_time,
        cp->max_recruiting_time(), max::max_recruiting_time);
    recruiting_time_ = new IntervalWidget(min_recruiting_time_, max_recruiting_time_);
    item(tr("thechess.competition.recruiting_time"), "",
        min_recruiting_time_->form_widget(), recruiting_time_);

    max_simultaneous_games_ = new MySpinBox();
    max_simultaneous_games_->setRange(min::max_simultaneous_games, max::max_simultaneous_games);
    max_simultaneous_games_->setValue(cp->max_simultaneous_games());
    item(tr("thechess.competition.max_simultaneous_games"), "",
        max_simultaneous_games_, max_simultaneous_games_);

    games_factor_ = new MyDoubleSpinBox();
    games_factor_->setRange(min::games_factor, max::games_factor);
    games_factor_->setValue(cp->games_factor());
    item(tr("thechess.competition.games_factor"), "", games_factor_, games_factor_);

    relax_time_ = new TimeDeltaWidget(min::relax_time,
        cp->relax_time(), max::relax_time);
    item(tr("thechess.competition.relax_time"), "",
        relax_time_->form_widget(), relax_time_);

    type_handler_();
}

void CompetitionParametersWidget::apply_parameters(CompetitionParameters* cp)
{
    GameParametersWidget::apply_parameters(cp);
    CompetitionType t = get_type_();
    cp->set_type(t);
    cp->set_min_rating(min_rating_->value());
    cp->set_max_rating(max_rating_->value());
    cp->set_min_classification(min_classification_->value());
    cp->set_max_classification(max_classification_->value());
    cp->set_force_start_delay(force_start_delay_->value());
    if (t == CLASSICAL || t == STAGED)
    {
        cp->set_min_users(min_users_->value());
        cp->set_max_users(max_users_->value());
        cp->set_min_recruiting_time(min_recruiting_time_->value());
        cp->set_max_recruiting_time(max_recruiting_time_->value());
    }
    if (t == CLASSICAL)
    {
        cp->set_max_simultaneous_games(max_simultaneous_games_->value());
        cp->set_games_factor(games_factor_->value());
    }
    if (t == STAGED)
    {
        cp->set_relax_time(relax_time_->value());
    }

    if (cp->min_rating() > cp->max_rating())
    {
        cp->set_max_rating(cp->min_rating());
    }
    if (cp->min_classification() > cp->max_classification())
    {
        cp->set_max_classification(cp->min_classification());
    }
    if (t == CLASSICAL || t == STAGED)
    {
        if (cp->min_users() > cp->max_users())
        {
            cp->set_min_users(cp->max_users());
        }
        if (cp->min_recruiting_time() > cp->max_recruiting_time())
        {
            cp->set_min_recruiting_time(cp->max_recruiting_time());
        }
    }
}

void CompetitionParametersWidget::type_handler_()
{
    CompetitionType t = get_type_();
    hide(users_);
    hide(recruiting_time_);
    hide(max_simultaneous_games_);
    hide(games_factor_);
    hide(relax_time_);
    if (t == CLASSICAL || t == STAGED)
    {
        show(users_);
        show(recruiting_time_);
    }
    if (t == CLASSICAL)
    {
        show(max_simultaneous_games_);
        show(games_factor_);
    }
    if (t == STAGED)
    {
        show(relax_time_);
    }
}

void CompetitionParametersWidget::set_type_(CompetitionType t)
{
    if (t == CLASSICAL)
    {
        type_->setCurrentIndex(0);
    }
    else if (t == STAGED)
    {
        type_->setCurrentIndex(1);
    }
    else if (t == TEAM)
    {
        type_->setCurrentIndex(2);
    }
}

CompetitionType CompetitionParametersWidget::get_type_() const
{
    CompetitionType t = defaults::competition_type;
    int index = type_->currentIndex();
    if (index == 0)
    {
        t = CLASSICAL;
    }
    else if (index == 1)
    {
        t = STAGED;
    }
    else if (index == 2)
    {
        t = TEAM;
    }
    return t;
}

}
}

