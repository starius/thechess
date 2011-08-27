/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

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

GameParametersWidget::GameParametersWidget(const GameParameters* gp,
        Wt::WContainerWidget* parent) :
    TableForm(parent) {
    using namespace config; // max, min
    Wt::WContainerWidget* cell;
    section(tr("tc.game.Parameters"));
    cell = item(tr("tc.game.Start_position"), "", 0, 0, false);
    moves_widget_ = new MovesWidget(gp->moves(), false, true,
                                    max::MOVES_INIT, false, WHITE, cell);
    Wt::WPushButton* moves_reset_ = new Wt::WPushButton(tr("tc.common.Reset"), cell);
    moves_reset_->clicked().connect(moves_widget_, &MovesWidget::reset);
    limit_std_ = new TimeDeltaWidget(min::LIMIT_STD, gp->limit_std(), max::LIMIT_STD);
    item(tr("tc.game.limit_std"), "", limit_std_->form_widget(), limit_std_);
    limit_private_init_ = new TimeDeltaWidget(min::LIMIT_PRIVATE_INIT,
            gp->limit_private_init(), max::LIMIT_PRIVATE_INIT);
    item(tr("tc.game.limit_private_init"), "",
         limit_private_init_->form_widget(), limit_private_init_);
    pause_limit_init_ = new TimeDeltaWidget(min::PAUSE_LIMIT_INIT,
                                            gp->pause_limit_init(), max::PAUSE_LIMIT_INIT);
    item(tr("tc.game.pause_limit"), "",
         pause_limit_init_->form_widget(), pause_limit_init_);
    norating_ = new Wt::WCheckBox();
    item(tr("tc.game.norating"), "", norating_, norating_);
    norating_->setCheckState(gp->norating() ? Wt::Checked : Wt::Unchecked);
    first_draw_ = new MySpinBox();
    first_draw_->setRange(min::FIRST_DRAW / 2, max::FIRST_DRAW / 2);
    first_draw_->setValue(gp->first_draw() / 2);
    item(tr("tc.game.first_draw"), "", first_draw_, first_draw_);
}

void GameParametersWidget::apply_parameters(GameParameters* gp) {
    gp->set_init_moves(moves_widget_->moves());
    gp->set_limit_std(limit_std_->value());
    gp->set_limit_private_init(limit_private_init_->value());
    gp->set_norating(norating_->checkState() == Wt::Checked);
    gp->set_pause_limit_init(pause_limit_init_->value());
    gp->set_first_draw(first_draw_->value() * 2);
}

}

