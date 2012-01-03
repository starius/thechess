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

#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "widgets/MyGamesList.hpp"
#include "widgets/GameWidget.hpp"
#include "widgets/GameCreateWidget.hpp"
#include "widgets/GameListWidget.hpp"
#include "widgets/CompetitionWidget.hpp"
#include "widgets/CompetitionCreateWidget.hpp"
#include "widgets/CompetitionListWidget.hpp"
#include "widgets/UserWidget.hpp"

namespace thechess {

MainWidget::MainWidget(Wt::WContainerWidget* parent):
    Wt::WContainerWidget(parent),
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
    // TODO MyGamesList
}

void MainWidget::show_menu(Path* path) {
    l()->addWidget(new MainMenu(path), Wt::WBorderLayout::West);
}

void MainWidget::user_view(const UserPtr& user) {
    set_contents(new UserWidget(user));
}

void MainWidget::user_list() {
    // TODO
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

