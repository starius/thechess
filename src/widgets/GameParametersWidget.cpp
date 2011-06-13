
#include <Wt/WLabel>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WTable>

#include "widgets/GameParametersWidget.hpp"
#include "widgets/MySpinBox.hpp"
#include "widgets/MovesWidget.hpp"
#include "widgets/TimeDeltaWidget.hpp"
#include "config.hpp"

namespace thechess {
namespace widgets {
using namespace model;

GameParametersWidget::GameParametersWidget(const GameParameters* gp,
    Wt::WContainerWidget* parent) :
TableForm(parent)
{
    using namespace config; // max, min

    Wt::WContainerWidget* cell;

    cell = item(tr("thechess.start_position"), "", 0, 0, false);
    moves_widget_ = new MovesWidget(gp->moves(), false, true,
        max::moves_init, false, chess::white, cell);
    Wt::WPushButton* moves_reset_ = new Wt::WPushButton(tr("thechess.reset"), cell);
    moves_reset_->clicked().connect(moves_widget_, &MovesWidget::reset);

    limit_std_ = new TimeDeltaWidget(min::limit_std, gp->limit_std(), max::limit_std);
    item(tr("thechess.limit_std"), "", limit_std_->form_widget(), limit_std_);

    limit_private_init_ = new TimeDeltaWidget(min::limit_private_init,
        gp->limit_private_init(), max::limit_private_init);
    item(tr("thechess.limit_private_init"), "",
        limit_private_init_->form_widget(), limit_private_init_);

    pause_limit_init_ = new TimeDeltaWidget(min::pause_limit_init,
        gp->pause_limit_init(), max::pause_limit_init);
    item(tr("thechess.pause_limit"), "",
        pause_limit_init_->form_widget(), pause_limit_init_);

    norating_ = new Wt::WCheckBox();
    item(tr("thechess.norating"), "", norating_, norating_);
    norating_->setCheckState(gp->norating() ? Wt::Checked : Wt::Unchecked);

    first_draw_ = new MySpinBox();
    first_draw_->setRange(min::first_draw / 2, max::first_draw / 2);
    first_draw_->setValue(gp->first_draw() / 2);
    item(tr("thechess.first_draw"), "", first_draw_, first_draw_);
}

void GameParametersWidget::apply_parameters(GameParameters* gp)
{
    gp->set_init_moves(moves_widget_->moves());
    gp->set_limit_std(limit_std_->timedelta());
    gp->set_limit_private_init(limit_private_init_->timedelta());
    gp->set_norating(norating_->checkState() == Wt::Checked);
    gp->set_pause_limit_init(pause_limit_init_->timedelta());
    gp->set_first_draw(first_draw_->value() * 2);
}

}
}

