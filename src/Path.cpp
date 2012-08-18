/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/assert.hpp>
#include <boost/bind.hpp>

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
    virtuals_of_user_ = new PredefinedNode("virtuals", user_view_);
    games_of_user_ = new PredefinedNode("games", user_view_);
    competitions_of_user_ = new PredefinedNode("competitions", user_view_);
    all_virtuals_ = new PredefinedNode("virtuals", user_list_);
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
    PredefinedNode* forum = new PredefinedNode("forum", this);
    topics_ = new PredefinedNode("topic", forum);
    topic_posts_ = new IntegerNode(topics_);
    all_posts_ = new PredefinedNode("post", forum);
    post_ = new IntegerNode(all_posts_);
    PredefinedNode* forum_comment = new PredefinedNode("comment", forum);
    post_comment_ = new IntegerNode(forum_comment);
    PredefinedNode* forum_edit = new PredefinedNode("edit", forum);
    forum_edit_ = new IntegerNode(forum_edit);
    PredefinedNode* comment = new PredefinedNode("comment", this);
    chat_comment_ = new IntegerNode(comment);
    tApp->internalPathChanged().connect(this, &Parser::open);
}

void Path::connect_main_widget(MainWidget* mw) {
    main_widget_ = mw;
    connect(user_list_, boost::bind(&MainWidget::user_list, mw));
    connect(user_view_, boost::bind(&Path::open_user, this));
    connect(virtuals_of_user_, boost::bind(&Path::open_virtuals_of_user, this));
    connect(games_of_user_, boost::bind(&Path::open_games_of_user, this));
    connect(competitions_of_user_, boost::bind(&Path::open_competitions_of_user,
            this));
    connect(all_virtuals_, boost::bind(&MainWidget::all_virtuals, mw));
    connect(settings_page_, boost::bind(&MainWidget::settings_page, mw));
    connect(game_list_, boost::bind(&MainWidget::game_list, mw));
    connect(game_view_, boost::bind(&Path::open_game, this));
    connect(game_new_, boost::bind(&MainWidget::game_new, mw));
    connect(gp_list_, boost::bind(&MainWidget::gp_list, mw));
    connect(competition_list_, boost::bind(&MainWidget::competition_list, mw));
    connect(competition_view_, boost::bind(&Path::open_competition, this));
    connect(competition_new_, boost::bind(&MainWidget::competition_new, mw));
    connect(board_, boost::bind(&Path::open_board, this));
    connect(board_root_, boost::bind(&MainWidget::moves_widget, mw));
    connect(topics_, boost::bind(&MainWidget::forum_topics, mw));
    connect(topic_posts_, boost::bind(&Path::open_forum_topic_posts, this));
    connect(all_posts_, boost::bind(&MainWidget::forum_all_posts, mw));
    connect(post_, boost::bind(&Path::open_forum_post, this));
    connect(post_comment_, boost::bind(&Path::open_forum_post_comment, this));
    connect(forum_edit_, boost::bind(&Path::open_forum_edit, this));
    connect(chat_comment_, boost::bind(&Path::open_chat_comment, this));
}

void Path::open_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->user_view(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_virtuals_of_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->virtuals_of_user(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_games_of_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->games_of_user(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_competitions_of_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->competitions_of_user(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_game() {
    BOOST_ASSERT(main_widget_);
    long long id = game_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->game_view(tApp->session().load<Game>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_competition() {
    BOOST_ASSERT(main_widget_);
    long long id = competition_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->competition_view(tApp->session().load<Competition>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_board() {
    main_widget_->board_view(board_->string());
}

void Path::open_forum_topic_posts() {
    BOOST_ASSERT(main_widget_);
    long long id = topic_posts_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_topic_posts(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_forum_post() {
    BOOST_ASSERT(main_widget_);
    long long id = post_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_post(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_forum_post_comment() {
    BOOST_ASSERT(main_widget_);
    long long id = post_comment_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_post_comment(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_forum_edit() {
    BOOST_ASSERT(main_widget_);
    long long id = forum_edit_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_edit(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_chat_comment() {
    BOOST_ASSERT(main_widget_);
    long long id = chat_comment_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->chat_comment(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

}

