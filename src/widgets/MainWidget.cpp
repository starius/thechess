/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WBorderLayout>
#include <Wt/WImage>
#include <Wt/WTable>
#include <Wt/Auth/AuthWidget>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/SWFStore.hpp>

#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "widgets/game/MyGamesList.hpp"
#include "widgets/game/GameWidget.hpp"
#include "widgets/game/GameCreateWidget.hpp"
#include "widgets/game/GameListWidget.hpp"
#include "widgets/gp/GPListWidget.hpp"
#include "widgets/competition/CompetitionWidget.hpp"
#include "widgets/competition/CompetitionCreateWidget.hpp"
#include "widgets/competition/CompetitionListWidget.hpp"
#include "widgets/user/UserWidget.hpp"

namespace thechess {

MainWidget::MainWidget(Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
    swfstore_container_(0),
    auth_widget_container_(0),
    auth_widget_(0) {
    setLayout(new Wt::WBorderLayout(), Wt::AlignTop | Wt::AlignJustify);
    Wt::WTable* top = new Wt::WTable();
    l()->addWidget(top, Wt::WBorderLayout::North);
    top->elementAt(0, 0)->addWidget(new Wt::WImage("img/top.gif"));
    auth_widget_container_ = new Wt::WContainerWidget();
    top->elementAt(0, 1)->addWidget(auth_widget_container_);
    top->elementAt(0, 1)->setContentAlignment(Wt::AlignRight);
    l()->addWidget(new Wt::WContainerWidget(), Wt::WBorderLayout::Center);
    l()->addWidget(new Wt::WContainerWidget(), Wt::WBorderLayout::East);
    swfstore_container_ = new Wt::WContainerWidget();
    l()->addWidget(swfstore_container_, Wt::WBorderLayout::South);
}

void MainWidget::show_menu(Path* path) {
    l()->addWidget(new MainMenu(path), Wt::WBorderLayout::West);
}

MainMenu* MainWidget::main_menu() {
    return downcast<MainMenu*>(l()->widgetAt(Wt::WBorderLayout::West));
}

void MainWidget::update_my_games() {
    Wt::WWidget* east_widget = l()->widgetAt(Wt::WBorderLayout::East);
    Wt::WContainerWidget* east = downcast<Wt::WContainerWidget*>(east_widget);
    east->clear();
    if (tApp->user()) {
        east->addWidget(new MyGamesList(tApp->user()));
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

void MainWidget::set_auth_widget(Wt::Auth::AuthWidget* widget) {
    auth_widget_ = widget;
    auth_widget_container_->clear();
    auth_widget_container_->addWidget(auth_widget_);
}

Wt::Wc::SWFStore* MainWidget::swf_store() {
    return downcast<Wt::Wc::SWFStore*>(swfstore_container_->widget(0));
}

void MainWidget::set_swfstore(Wt::Wc::SWFStore* swfstore) {
    swfstore_container_->clear();
    swfstore_container_->addWidget(swfstore);
}

Wt::WBorderLayout* MainWidget::l() {
    return downcast<Wt::WBorderLayout*>(layout());
}

Wt::WContainerWidget* MainWidget::contents() {
    WWidget* widget = l()->widgetAt(Wt::WBorderLayout::Center);
    return downcast<Wt::WContainerWidget*>(widget);
}

void MainWidget::set_contents(WWidget* widget) {
    contents()->clear();
    contents()->addWidget(widget);
}

}

