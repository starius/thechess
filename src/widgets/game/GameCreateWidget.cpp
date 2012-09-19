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
#include <Wt/Wc/TableForm.hpp>

#include "widgets/game/GameCreateWidget.hpp"
#include "widgets/gp/GPSelector.hpp"
#include "widgets/Header.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "notify.hpp"

namespace thechess {

GameCreateWidget::GameCreateWidget(const UserPtr& user,
                                   Wt::WContainerWidget* p) :
    Wt::WContainerWidget(p), with_user_(true), user_(user) {
    dbo::Transaction t(tApp->session());
    tApp->user().reread();
    new Header(tr("tc.game.Competitor")
               .arg(user->username()), this);
    if (User::is_blocked(tApp->user(), user_)) {
        new Wt::WText(tr("tc.user.User_blocked_you"), this);
        return;
    }
    if (tApp->user() && tApp->user()->has_permission(GAME_CREATOR)) {
        print();
    }
}

GameCreateWidget::GameCreateWidget(Wt::WContainerWidget* p) :
    Wt::WContainerWidget(p), with_user_(false) {
    dbo::Transaction t(tApp->session());
    new Header(tr("tc.game.Challenge"), this);
    tApp->user().reread();
    if (tApp->user() && tApp->user()->has_permission(GAME_CREATOR)) {
        print();
    }
}

void GameCreateWidget::print() {
    selector_ = new GPSelector(this);
    Wt::Wc::TableForm* form = new Wt::Wc::TableForm(this);
    color_ = new Wt::WComboBox();
    color_->addItem(tr("tc.game.random"));
    color_->addItem(tr("tc.game.white"));
    color_->addItem(tr("tc.game.black"));
    form->item(tr("tc.game.your_color"), "", color_, color_);
    new Wt::WBreak(this);
    Wt::WPushButton* ok = new Wt::WPushButton(tr("tc.common.Create"), this);
    ok->clicked().connect(this, &GameCreateWidget::button_handler);
    error_message_ = new Wt::WText(this);
    error_message_->setStyleClass("thechess-error");
}

void GameCreateWidget::button_handler() {
    dbo::Transaction t(tApp->session());
    GPPtr gp = selector_->gp();
    if (!gp) {
        error_message_->setText(tr("tc.common.no_params_error"));
        return;
    }
    tApp->user().reread();
    if (!tApp->user() || !tApp->user()->has_permission(GAME_CREATOR)) {
        return;
    }
    error_message_->setText("");
    GamePtr game = tApp->session().add(new Game(gp));
    Piece::Color color = selected_color();
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
    tApp->server().planning().add(new Object(GAME, game.id()), now());
}

Piece::Color GameCreateWidget::selected_color() const {
    Piece::Color color = Piece::COLOR_NULL;
    if (color_->currentIndex() == 1) {
        color = Piece::WHITE;
    } else if (color_->currentIndex() == 2) {
        color = Piece::BLACK;
    }
    return color;
}

}

