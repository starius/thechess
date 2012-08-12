/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTable>
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
#include "widgets/user/SettingsWidget.hpp"
#include "widgets/comment/CommentList.hpp"

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
    Wt::WTable* top = new Wt::WTable(this);
    top->elementAt(0, 0)->addWidget(new Wt::WText(tr("tc.common.Logo")));
    auth_place_ = top->elementAt(0, AUTH_IN_TOP);
    Wt::WTable* middle = new Wt::WTable(this);
    middle->resize(Wt::WLength(100, Wt::WLength::Percentage),
                   Wt::WLength::Auto);
    menu_place_ = middle->elementAt(0, MENU_IN_MIDDLE);
    contents_place_ = middle->elementAt(0, CONTENTS_IN_MIDDLE);
    contents_place_->resize(Wt::WLength(80, Wt::WLength::Percentage),
                            Wt::WLength::Auto);
    mygames_place_ = middle->elementAt(0, GAME_LIST_IN_MIDDLE);
    bottom_place_ = new Wt::WContainerWidget(this);
}

void MainWidget::show_menu(Path* path) {
    menu_place_->addWidget(new MainMenu(path));
}

MainMenu* MainWidget::main_menu() {
    return downcast<MainMenu*>(menu_place_->widget(0));
}

void MainWidget::update_my_games() {
    mygames_place_->clear();
    if (tApp->user()) {
        mygames_place_->addWidget(new MyGamesList(tApp->user()));
    }
}

void MainWidget::user_view(const UserPtr& user) {
    set_contents(new UserWidget(user));
}

void MainWidget::settings_page() {
    if (tApp->user()) {
        set_contents(new SettingsWidget());
    }
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
    BoardWidget* b;
    b = new BoardWidget(/* big */ false, /* active */ true,
                                  /* bottom */ Piece::WHITE, Board(data));
    b->show_lastmove_checkbox(false);
    set_contents(b);
}

void MainWidget::forum_topics() {
    set_contents(new CommentList(Comment::FORUM_TOPIC));
}

void MainWidget::forum_topic_posts(const CommentPtr& topic) {
    set_contents(new CommentList(Comment::FORUM_POST, topic));
}

void MainWidget::forum_all_posts() {
    set_contents(new CommentList(Comment::FORUM_POST));
}

void MainWidget::forum_post(const CommentPtr& post) {
    if (post->children().size()) {
        CommentPtr post_text = post->children().front();
        set_contents(new CommentList(Comment::FORUM_COMMENT, post_text));
    }
}

Wt::Auth::AuthWidget* MainWidget::auth_widget() {
    return downcast<Wt::Auth::AuthWidget*>(auth_place_->widget(0));
}

void MainWidget::set_auth_widget(Wt::Auth::AuthWidget* widget) {
    auth_place_->clear();
    auth_place_->addWidget(widget);
}

Wt::Wc::SWFStore* MainWidget::swf_store() {
    return downcast<Wt::Wc::SWFStore*>(bottom_place_->widget(0));
}

void MainWidget::set_swfstore(Wt::Wc::SWFStore* swfstore) {
    swfstore->setMaximumSize(1, 1);
    bottom_place_->clear();
    bottom_place_->addWidget(swfstore);
}

void MainWidget::set_contents(WWidget* widget) {
    contents_place_->clear();
    contents_place_->addWidget(widget);
}

}

