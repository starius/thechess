/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WDateTime>
#include <Wt/Auth/AuthWidget>
#include <Wt/Wc/util.hpp>
#include <Wt/Wc/SWFStore.hpp>
#include <Wt/Wc/Countdown.hpp>

#include "model/all.hpp"
#include "widgets/MainWidget.hpp"
#include "widgets/MainMenu.hpp"
#include "widgets/game/MyGamesList.hpp"
#include "widgets/chess/BoardWidget.hpp"
#include "widgets/chess/MovesWidget.hpp"
#include "widgets/game/GameWidget.hpp"
#include "widgets/game/GameCreateWidget.hpp"
#include "widgets/game/GameListWidget.hpp"
#include "widgets/gp/GPListWidget.hpp"
#include "widgets/competition/CompetitionWidget.hpp"
#include "widgets/competition/CompetitionCreateWidget.hpp"
#include "widgets/competition/CompetitionListWidget.hpp"
#include "widgets/user/UserWidget.hpp"
#include "widgets/user/UserListWidget.hpp"
#include "widgets/user/VirtualsWidget.hpp"
#include "widgets/user/SettingsWidget.hpp"
#include "widgets/comment/CommentList.hpp"
#include "widgets/comment/ForumCommentWidget.hpp"
#include "widgets/comment/ForumEdit.hpp"
#include "widgets/comment/ChatCommentWidget.hpp"

namespace thechess {

const int AUTH_IN_TOP = 1;

enum {
    MENU_IN_MIDDLE,
    CONTENTS_IN_MIDDLE,
    GAME_LIST_IN_MIDDLE
};

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
    show_countup();
}

void MainWidget::show_menu(Path* path) {
    menu_place_->addWidget(new MainMenu(path));
}

MainMenu* MainWidget::main_menu() {
    return downcast<MainMenu*>(menu_place_->widget(1));
}

void MainWidget::update_my_games() {
    mygames_place_->clear();
    if (tApp->user()) {
        mygames_place_->addWidget(new MyGamesList(tApp->user()));
    }
}

void MainWidget::user_view(const UserPtr& user) {
    set_contents(new UserWidget(user));
    wApp->setTitle(tr("tc.title.UserWidget").arg(user->username20()));
}

void MainWidget::virtuals_of_user(const UserPtr& user) {
    set_contents(new VirtualsWidget(user));
    wApp->setTitle(tr("tc.title.VirtualsWidget_of").arg(user->username20()));
}

void MainWidget::games_of_user(const UserPtr& user) {
    set_contents(new GameListWidget(user));
    wApp->setTitle(tr("tc.title.GameListWidget_of").arg(user->username20()));
}

void MainWidget::competitions_of_user(const UserPtr& user) {
    set_contents(new CompetitionListWidget(user));
    wApp->setTitle(tr("tc.title.CompetitionListWidget_of")
                   .arg(user->username20()));
}

void MainWidget::all_virtuals() {
    set_contents(new VirtualsWidget());
    wApp->setTitle(tr("tc.title.VirtualsWidget"));
}

void MainWidget::settings_page() {
    if (tApp->user()) {
        set_contents(new SettingsWidget());
        wApp->setTitle(tr("tc.title.SettingsWidget"));
    }
}

void MainWidget::user_list() {
    set_contents(new UserListWidget());
    wApp->setTitle(tr("tc.title.UserListWidget"));
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
    wApp->setTitle(tr("tc.title.GameWidget").arg(game.id()));
}

void MainWidget::game_list() {
    set_contents(new GameListWidget());
    wApp->setTitle(tr("tc.title.GameListWidget"));
}

void MainWidget::game_new() {
    set_contents(new GameCreateWidget());
    wApp->setTitle(tr("tc.title.GameCreateWidget"));
}

void MainWidget::gp_list() {
    set_contents(new GPListWidget());
    wApp->setTitle(tr("tc.title.GPListWidget"));
}

void MainWidget::competition_view(const CompetitionPtr& competition) {
    set_contents(new CompetitionWidget(competition));
    wApp->setTitle(tr("tc.title.CompetitionWidget").arg(competition.id()));
}

void MainWidget::competition_list() {
    set_contents(new CompetitionListWidget());
    wApp->setTitle(tr("tc.title.CompetitionListWidget"));
}

void MainWidget::competition_new() {
    set_contents(new CompetitionCreateWidget());
    wApp->setTitle(tr("tc.title.CompetitionCreateWidget"));
}

void MainWidget::board_view(const std::string& data) {
    BoardWidget* b;
    b = new BoardWidget(/* big */ false, /* active */ true,
                                  /* bottom */ Piece::WHITE, Board(data));
    b->show_lastmove_checkbox(false);
    set_contents(b);
    wApp->setTitle(tr("tc.title.BoardWidget"));
}

void MainWidget::moves_widget() {
    set_contents(new MovesWidget(Moves(), /* big */ false, /* active */ true));
    wApp->setTitle(tr("tc.title.MovesWidget"));
}

void MainWidget::forum_topics() {
    set_contents(new CommentList(Comment::FORUM_TOPIC));
    wApp->setTitle(tr("tc.title.CommentList_topics"));
}

void MainWidget::forum_topic_posts(const CommentPtr& topic) {
    set_contents(new CommentList(Comment::FORUM_POST, topic));
    wApp->setTitle(tr("tc.title.CommentList_topic").arg(topic->text()));
}

void MainWidget::forum_all_posts() {
    set_contents(new CommentList(Comment::FORUM_POST));
    wApp->setTitle(tr("tc.title.CommentList_posts"));
}

void MainWidget::forum_post(const CommentPtr& post) {
    if (post->children().size()) {
        CommentPtr post_text = post->children().front();
        set_contents(new CommentList(Comment::FORUM_COMMENT, post_text));
        wApp->setTitle(tr("tc.title.CommentList_post").arg(post->text()));
    }
}

void MainWidget::forum_post_comment(const CommentPtr& comment) {
    CommentPtr post_text = comment->root();
    set_contents(new ForumCommentWidget(comment));
    wApp->setTitle(tr("tc.title.CommentList_forum_comment").arg(comment.id()));
}

void MainWidget::forum_edit(const CommentPtr& post_or_text) {
    set_contents(new ForumEdit(post_or_text));
    wApp->setTitle(tr("tc.title.CommentList_edit"));
}

void MainWidget::chat_comment(const CommentPtr& comment) {
    set_contents(new ChatCommentWidget(comment));
    wApp->setTitle(tr("tc.title.ChatCommentWidget"));
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

static void reset_countup(Wt::Wc::Countdown* countup) {
    Wt::WDateTime midnight = Wt::WDateTime(now().date());
    countup->set_since(midnight);
    countup->set_format("HM");
    Wt::WDateTime next_midnight = midnight.addDays(1);
    Td wait = next_midnight - now() + SECOND;
    Wt::Wc::schedule_action(wait, Wt::Wc::bound_post(boost::bind(reset_countup,
                            countup)));
    Wt::Wc::updates_trigger();
}

void MainWidget::show_countup() {
    Wt::Wc::Countdown* countup = new Wt::Wc::Countdown(menu_place_);
    reset_countup(countup);
}

}

