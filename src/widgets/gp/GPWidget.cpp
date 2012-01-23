/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WLabel>
#include <Wt/WText>
#include <Wt/WPushButton>
#include <Wt/WTable>
#include <Wt/Wc/ConstrainedSpinBox.hpp>
#include <Wt/Wc/TimeDurationWidget.hpp>

#include "widgets/gp/GPWidget.hpp"
#include "widgets/chess/MovesWidget.hpp"
#include "config.hpp"

namespace thechess {

GPWidget::GPWidget(const GP* gp,
                   Wt::WContainerWidget* parent) :
    Wt::Wc::TableForm(parent) {
    using namespace config; // max, min
    using namespace Wt::Wc;
    GP default_gp(true);
    if (!gp) {
        gp = &default_gp;
    }
    Wt::WContainerWidget* cell;
    section(tr("tc.game.Parameters"));
    cell = item(tr("tc.game.Start_position"), "", 0, 0, false);
    moves_widget_ = new MovesWidget(gp->moves(), false, true,
                                    max::MOVES_INIT, false, Piece::WHITE, cell);
    Wt::WPushButton* reset = new Wt::WPushButton(tr("tc.common.Reset"), cell);
    reset->clicked().connect(moves_widget_, &MovesWidget::reset);
    limit_std_ = new TimeDurationWidget(min::LIMIT_STD,
                                        gp->limit_std(),
                                        max::LIMIT_STD);
    item(tr("tc.game.limit_std"), "", limit_std_->form_widget(), limit_std_);
    limit_private_init_ = new TimeDurationWidget(min::LIMIT_PRIVATE_INIT,
            gp->limit_private_init(), max::LIMIT_PRIVATE_INIT);
    item(tr("tc.game.limit_private_init"), "",
         limit_private_init_->form_widget(), limit_private_init_);
    pause_limit_init_ = new TimeDurationWidget(min::PAUSE_LIMIT_INIT,
            gp->pause_limit_init(),
            max::PAUSE_LIMIT_INIT);
    item(tr("tc.game.pause_limit"), "",
         pause_limit_init_->form_widget(), pause_limit_init_);
    norating_ = new Wt::WCheckBox();
    item(tr("tc.game.norating"), "", norating_, norating_);
    norating_->setCheckState(gp->norating() ? Wt::Checked : Wt::Unchecked);
    first_draw_ = new ConstrainedSpinBox();
    first_draw_->setRange(min::FIRST_DRAW / 2, max::FIRST_DRAW / 2);
    first_draw_->setValue(gp->first_draw() / 2);
    item(tr("tc.game.first_draw"), "", first_draw_, first_draw_);
    add_record_inputs(gp, this);
}

void GPWidget::apply_parameters(GP* gp) {
    write_record(gp);
    gp->set_moves(moves_widget_->moves());
    gp->set_limit_std(limit_std_->corrected_value());
    gp->set_limit_private_init(limit_private_init_->corrected_value());
    gp->set_norating(norating_->checkState() == Wt::Checked);
    gp->set_pause_limit(pause_limit_init_->corrected_value());
    gp->set_first_draw(first_draw_->value() * 2);
}

}

