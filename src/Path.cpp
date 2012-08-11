/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>

#include "Path.hpp"
#include "Application.hpp"
#include "model/all.hpp"
#include "widgets/MainWidget.hpp"

namespace thechess {

using namespace Wt::Wc::url;

Path::Path(Wt::WObject* parent):
    Parser(parent), main_widget_(0) {
    user_list_ = new PredefinedNode("user", this);
    user_view_ = new IntegerNode(user_list_);
    settings_page_ = new PredefinedNode("settings", user_list_);
    game_list_ = new PredefinedNode("game", this);
    game_view_ = new IntegerNode(game_list_);
    game_new_ = new PredefinedNode("new", game_list_);
    gp_list_ = new PredefinedNode("parameters", game_list_);
    gp_view_ = new IntegerNode(gp_list_);
    gp_new_ = new PredefinedNode("new", gp_list_);
    competition_list_ = new PredefinedNode("competition", this);
    competition_view_ = new IntegerNode(competition_list_);
    competition_new_ = new PredefinedNode("new", competition_list_);
    cp_list_ = new PredefinedNode("parameters", competition_list_);
    cp_view_ = new IntegerNode(cp_list_);
    cp_new_ = new PredefinedNode("new", cp_list_);
    board_root_ = new PredefinedNode("board", this);
    board_ = new StringNode(board_root_);
    topics_ = new PredefinedNode("forum", this);
    PredefinedNode* topic = new PredefinedNode("topic", topics_);
    topic_posts_ = new IntegerNode(topic);
    PredefinedNode* post = new PredefinedNode("post", topics_);
    all_posts_ = new PredefinedNode("all", post);
    post_ = new IntegerNode(post);
    tApp->internalPathChanged().connect(this, &Parser::open);
}

void Path::connect_main_widget(MainWidget* mw) {
    main_widget_ = mw;
    // TODO user list
    user_view_->opened().connect(this, &Path::open_user);
    settings_page_->opened().connect(mw, &MainWidget::settings_page);
    game_list_->opened().connect(mw, &MainWidget::game_list);
    game_view_->opened().connect(this, &Path::open_game);
    game_new_->opened().connect(mw, &MainWidget::game_new);
    gp_list_->opened().connect(mw, &MainWidget::gp_list);
    competition_list_->opened().connect(mw, &MainWidget::competition_list);
    competition_view_->opened().connect(this, &Path::open_competition);
    competition_new_->opened().connect(mw, &MainWidget::competition_new);
    board_->opened().connect(this, &Path::open_board);
    topics_->opened().connect(mw, &MainWidget::forum_topics);
    topic_posts_->opened().connect(this, &Path::forum_topic_posts);
    all_posts_->opened().connect(mw, &MainWidget::forum_all_posts);
    post_->opened().connect(this, &Path::forum_post);
}

void Path::open_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->user_view(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_game() {
    BOOST_ASSERT(main_widget_);
    long long id = game_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->game_view(tApp->session().load<Game>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_competition() {
    BOOST_ASSERT(main_widget_);
    long long id = competition_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->competition_view(tApp->session().load<Competition>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_board() {
    main_widget_->board_view(board_->string());
}

void Path::forum_topic_posts() {
    BOOST_ASSERT(main_widget_);
    long long id = topic_posts_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_topic_posts(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::forum_post() {
    BOOST_ASSERT(main_widget_);
    long long id = post_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_post(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

}

