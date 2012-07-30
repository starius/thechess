/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WBorderLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/Auth/AuthWidget>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/SWFStore.hpp>

#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "widgets/game/MyGamesList.hpp"
#include "widgets/chess/BoardWidget.hpp"
#include "widgets/game/GameWidget.hpp"
#include "widgets/game/GameCreateWidget.hpp"
#include "widgets/game/GameListWidget.hpp"
#include "widgets/gp/GPListWidget.hpp"
#include "widgets/competition/CompetitionWidget.hpp"
#include "widgets/competition/CompetitionCreateWidget.hpp"
#include "widgets/competition/CompetitionListWidget.hpp"
#include "widgets/user/UserWidget.hpp"

namespace thechess {

const int TOP_IN_L = 0;
const int MIDDLE_IN_L = 1;
const int BOTTOM_IN_L = 2;

const int AUTH_IN_TOP = 1;

const int MENU_IN_MIDDLE = 0;
const int CONTENTS_IN_MIDDLE = 1;
const int GAME_LIST_IN_MIDDLE = 2;

MainWidget::MainWidget(Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent) {
    setLayout(new Wt::WVBoxLayout());
    Wt::WHBoxLayout* top_layout = new Wt::WHBoxLayout();
    l()->addLayout(top_layout);
    top_layout->addWidget(new Wt::WText(tr("tc.common.Logo")));
    top_layout->addLayout(new Wt::WVBoxLayout()); // auth
    top_layout->setStretchFactor(auth(), 1);
    Wt::WHBoxLayout* middle_layout = new Wt::WHBoxLayout();
    l()->addLayout(middle_layout);
    l()->setStretchFactor(middle_layout, 1);
    middle_layout->addLayout(new Wt::WVBoxLayout()); // menu
    middle_layout->addLayout(new Wt::WVBoxLayout()); // contents
    middle_layout->addLayout(new Wt::WVBoxLayout()); // my games
    middle_layout->setStretchFactor(contents(), 1);
    l()->addLayout(new Wt::WHBoxLayout()); // bottom
}

void MainWidget::show_menu(Path* path) {
    menu()->addWidget(new MainMenu(path));
}

MainMenu* MainWidget::main_menu() {
    return downcast<MainMenu*>(menu()->itemAt(0)->widget());
}

void MainWidget::update_my_games() {
    my_games()->clear();
    if (tApp->user()) {
        my_games()->addWidget(new MyGamesList(tApp->user()));
    }
}

void MainWidget::user_view(const UserPtr& user) {
    set_contents(new UserWidget(user));
}

void MainWidget::user_list() {
    // TODO
}

void MainWidget::update_password() {
    const Wt::Auth::User& user = auth_widget()->login().user();
    if (user.isValid()) {
        auth_widget()->letUpdatePassword(user, /* promptPassword */ true);
        // FIXME Freezes, memory leak
    }
}

void MainWidget::game_view(const GamePtr& game) {
    set_contents(new GameWidget(game));
}

void MainWidget::game_list() {
    set_contents(new GameListWidget());
}

void MainWidget::game_new() {
    set_contents(new GameCreateWidget());
}

void MainWidget::gp_list() {
    set_contents(new GPListWidget());
}

void MainWidget::competition_view(const CompetitionPtr& competition) {
    set_contents(new CompetitionWidget(competition));
}

void MainWidget::competition_list() {
    set_contents(new CompetitionListWidget());
}

void MainWidget::competition_new() {
    set_contents(new CompetitionCreateWidget());
}

void MainWidget::board_view(const std::string& data) {
    set_contents(new BoardWidget(/* big */ false, /* active */ true,
                                           /* bottom */ Piece::WHITE,
                                           Board(data)));
}

Wt::Auth::AuthWidget* MainWidget::auth_widget() {
    return downcast<Wt::Auth::AuthWidget*>(auth()->itemAt(0)->widget());
}

void MainWidget::set_auth_widget(Wt::Auth::AuthWidget* widget) {
    auth()->clear();
    auth()->addWidget(widget);
}

Wt::Wc::SWFStore* MainWidget::swf_store() {
    return downcast<Wt::Wc::SWFStore*>(bottom()->itemAt(0)->widget());
}

void MainWidget::set_swfstore(Wt::Wc::SWFStore* swfstore) {
    swfstore->setMaximumSize(1, 1);
    bottom()->clear();
    bottom()->addWidget(swfstore);
}

Wt::WVBoxLayout* MainWidget::l() {
    return downcast<Wt::WVBoxLayout*>(layout());
}

Wt::WHBoxLayout* MainWidget::top() {
    return downcast<Wt::WHBoxLayout*>(l()->itemAt(TOP_IN_L));
}

Wt::WHBoxLayout* MainWidget::middle() {
    return downcast<Wt::WHBoxLayout*>(l()->itemAt(MIDDLE_IN_L));
}

Wt::WHBoxLayout* MainWidget::bottom() {
    return downcast<Wt::WHBoxLayout*>(l()->itemAt(BOTTOM_IN_L));
}

Wt::WVBoxLayout* MainWidget::auth() {
    return downcast<Wt::WVBoxLayout*>(top()->itemAt(AUTH_IN_TOP));
}

Wt::WVBoxLayout* MainWidget::menu() {
    return downcast<Wt::WVBoxLayout*>(middle()->itemAt(MENU_IN_MIDDLE));
}

Wt::WVBoxLayout* MainWidget::contents() {
    return downcast<Wt::WVBoxLayout*>(middle()->itemAt(CONTENTS_IN_MIDDLE));
}

Wt::WVBoxLayout* MainWidget::my_games() {
    return downcast<Wt::WVBoxLayout*>(middle()->itemAt(GAME_LIST_IN_MIDDLE));
}

void MainWidget::set_contents(WWidget* widget) {
    contents()->clear();
    contents()->addWidget(widget);
}

}

