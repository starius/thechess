/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WComboBox>
#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/Dbo/Transaction>

#include "widgets/GameCreateWidget.hpp"
#include "widgets/GPWidget.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "notify.hpp"

namespace thechess {

GameCreateWidget::GameCreateWidget(const UserPtr& user,
                                   Wt::WContainerWidget* p) :
    Wt::WContainerWidget(p), with_user_(true), user_(user) {
    dbo::Transaction t(tApp->session());
    new Wt::WText(tr("tc.game.Competitor")
                  .arg(user->username()), this);
    if (tApp->user()) {
        print_();
    }
    t.commit();
}

GameCreateWidget::GameCreateWidget(Wt::WContainerWidget* p) :
    Wt::WContainerWidget(p), with_user_(false) {
    new Wt::WText(tr("tc.game.Challenge"), this);
    if (tApp->user()) {
        print_();
    }
}

void GameCreateWidget::print_() {
    GP gp(true);
    gpw_ = new GPWidget(&gp, this);
    color_ = new Wt::WComboBox();
    color_->addItem(tr("tc.game.random"));
    color_->addItem(tr("tc.game.white"));
    color_->addItem(tr("tc.game.black"));
    gpw_->item(tr("tc.game.your_color"), "", color_, color_);
    new Wt::WBreak(this);
    Wt::WPushButton* ok_ = new Wt::WPushButton(tr("tc.common.Create"), this);
    ok_->clicked().connect(this, &GameCreateWidget::button_handler_);
}

void GameCreateWidget::button_handler_() {
    dbo::Transaction t(tApp->session());
    GPPtr gp = tApp->session().add(new GP(true));
    GamePtr game = tApp->session().add(new Game(gp));
    gpw_->apply_parameters(gp.modify());
    Piece::Color color = selected_color_();
    if (with_user_) {
        game.modify()->propose_game(tApp->user(), user_, color);
    } else {
        game.modify()->propose_challenge(tApp->user(), color);
    }
    t.commit();
    tNot->emit(new Object(USER, tApp->user().id()));
    if (with_user_) {
        tNot->emit(new Object(USER, user_.id()));
    }
    tApp->path().game_view()->set_integer_value(game.id());
    tApp->path().game_view()->open(/* change path */ true);
}

Piece::Color GameCreateWidget::selected_color_() const {
    Piece::Color color = Piece::COLOR_NULL;
    if (color_->currentIndex() == 1) {
        color = Piece::WHITE;
    } else if (color_->currentIndex() == 2) {
        color = Piece::BLACK;
    }
    return color;
}

}

