/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>
#include <Wt/WComboBox>
#include <Wt/WLabel>
#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/GameCreateWidget.hpp"
#include "widgets/PleaseLoginWidget.hpp"
#include "widgets/GameParametersWidget.hpp"
#include "model/Game.hpp"
#include "ThechessApplication.hpp"
#include "ThechessNotifier.hpp"

namespace thechess {

GameCreateWidget::GameCreateWidget(UserPtr user,
                                   Wt::WContainerWidget* p) :
    Wt::WContainerWidget(p), with_user_(true), user_(user) {
    dbo::Transaction t(tApp->session());
    new Wt::WText(tr("tc.game.Competitor")
                  .arg(user->username()), this);
    if (!tApp->user()) {
        new PleaseLoginWidget(this);
    } else {
        print_();
    }
    t.commit();
}

GameCreateWidget::GameCreateWidget(Wt::WContainerWidget* p) :
    Wt::WContainerWidget(p), with_user_(false) {
    new Wt::WText(tr("tc.game.Challenge"), this);
    if (!tApp->user()) {
        new PleaseLoginWidget(this);
    } else {
        print_();
    }
}

void GameCreateWidget::print_() {
    GameParameters gp(true);
    gpw_ = new GameParametersWidget(&gp, this);
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
    GamePtr game = tApp->session().add(new Game(true));
    gpw_->apply_parameters(game.modify());
    Color color = selected_color_();
    if (with_user_) {
        game.modify()->propose_game(tApp->user(), user_, color);
    } else {
        game.modify()->propose_challenge(tApp->user(), color);
    }
    t.commit();
    ThechessNotifier::app_emit(Object(USER, tApp->user().id()));
    if (with_user_) {
        ThechessNotifier::app_emit(Object(USER, user_.id()));
    }
    tApp->view(game);
}

Color GameCreateWidget::selected_color_() const {
    Color color = COLOR_NULL;
    if (color_->currentIndex() == 1) {
        color = WHITE;
    } else if (color_->currentIndex() == 2) {
        color = BLACK;
    }
    return color;
}

}

