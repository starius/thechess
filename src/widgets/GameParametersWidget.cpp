
#include <Wt/WLabel>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WTable>

#include "widgets/GameParametersWidget.hpp"
#include "config.hpp"

namespace thechess {
namespace widgets {
using namespace model;

GameParametersWidget::GameParametersWidget(const GameParameters* gp,
    Wt::WContainerWidget* parent) :
Wt::WContainerWidget(parent)
{
    using namespace config; // max, min

    new Wt::WText(tr("thechess.start_position"), this);
    moves_widget_ = new MovesWidget(gp->moves(),
        false, true, max::moves_init, false, chess::white, this);
    Wt::WPushButton* moves_reset_ =
        new Wt::WPushButton(tr("thechess.reset"), this);
    moves_reset_->clicked().connect(moves_widget_, &MovesWidget::reset);

    Wt::WTable* t = new Wt::WTable(this);

    Wt::WLabel* limit_std_label =
        new Wt::WLabel(tr("thechess.limit_std"), t->elementAt(0, 0));
    limit_std_ = new TimeDeltaWidget(min::limit_std,
        gp->limit_std(), max::limit_std, t->elementAt(0,1));
    limit_std_label->setBuddy(limit_std_->form_widget());

    Wt::WLabel* limit_private_init_label =
        new Wt::WLabel(tr("thechess.limit_private_init"),
        t->elementAt(1,0));
    limit_private_init_ = new TimeDeltaWidget(min::limit_private_init,
        gp->limit_private_init(), max::limit_private_init,
        t->elementAt(1,1));
    limit_private_init_label
        ->setBuddy(limit_private_init_->form_widget());

    Wt::WLabel* pause_limit_label =
        new Wt::WLabel(tr("thechess.pause_limit"), t->elementAt(2,0));
    pause_limit_init_ = new TimeDeltaWidget(min::pause_limit_init,
        gp->pause_limit_init(), max::pause_limit_init, t->elementAt(2,1));
    pause_limit_label->setBuddy(pause_limit_init_->form_widget());

    norating_ = new Wt::WCheckBox(tr("thechess.norating"),
        t->elementAt(3,0));
    Wt::CheckState state = (gp->norating()) ?
        Wt::Checked : Wt::Unchecked;
    norating_->setCheckState(state);

    Wt::WLabel* first_draw_label =
        new Wt::WLabel(tr("thechess.first_draw"), t->elementAt(4,0));
    first_draw_ = new Wt::WSpinBox(t->elementAt(4,1));
    first_draw_->setRange(min::first_draw / 2, max::first_draw / 2);
    first_draw_->setValue(gp->first_draw() / 2);
    first_draw_label->setBuddy(first_draw_);
}

void GameParametersWidget::apply_parameters(GameParameters* gp)
{
    gp->set_init_moves(moves_widget_->moves());
    gp->set_limit_std(limit_std_->timedelta());
    gp->set_limit_private_init(limit_private_init_->timedelta());
    gp->set_norating(norating_->checkState() == Wt::Checked);
    gp->set_pause_limit_init(pause_limit_init_->timedelta());
    gp->set_first_draw(first_draw_value_());
}

int GameParametersWidget::first_draw_value_()
{
    int value = first_draw_->value();
    if (value < first_draw_->minimum())
    {
        value = first_draw_->minimum();
    }
    else if (value > first_draw_->maximum())
    {
        value = first_draw_->maximum();
    }
    return value * 2;
}

}
}

