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
    // TODO user list
    user_view_->opened().connect(this, &Path::open_user);
    virtuals_of_user_->opened().connect(this, &Path::open_virtuals_of_user);
    games_of_user_->opened().connect(this, &Path::open_games_of_user);
    competitions_of_user_->opened().connect(this,
                                            &Path::open_competitions_of_user);
    all_virtuals_->opened().connect(mw, &MainWidget::all_virtuals);
    settings_page_->opened().connect(mw, &MainWidget::settings_page);
    game_list_->opened().connect(mw, &MainWidget::game_list);
    game_view_->opened().connect(this, &Path::open_game);
    game_new_->opened().connect(mw, &MainWidget::game_new);
    gp_list_->opened().connect(mw, &MainWidget::gp_list);
    competition_list_->opened().connect(mw, &MainWidget::competition_list);
    competition_view_->opened().connect(this, &Path::open_competition);
    competition_new_->opened().connect(mw, &MainWidget::competition_new);
    board_->opened().connect(this, &Path::open_board);
    board_root_->opened().connect(mw, &MainWidget::moves_widget);
    topics_->opened().connect(mw, &MainWidget::forum_topics);
    topic_posts_->opened().connect(this, &Path::open_forum_topic_posts);
    all_posts_->opened().connect(mw, &MainWidget::forum_all_posts);
    post_->opened().connect(this, &Path::open_forum_post);
    post_comment_->opened().connect(this, &Path::open_forum_post_comment);
    forum_edit_->opened().connect(this, &Path::open_forum_edit);
    chat_comment_->opened().connect(this, &Path::open_chat_comment);
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

void Path::open_virtuals_of_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->virtuals_of_user(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_games_of_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->games_of_user(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_competitions_of_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->competitions_of_user(tApp->session().load<User>(id));
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

void Path::open_forum_topic_posts() {
    BOOST_ASSERT(main_widget_);
    long long id = topic_posts_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_topic_posts(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_forum_post() {
    BOOST_ASSERT(main_widget_);
    long long id = post_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_post(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_forum_post_comment() {
    BOOST_ASSERT(main_widget_);
    long long id = post_comment_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_post_comment(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_forum_edit() {
    BOOST_ASSERT(main_widget_);
    long long id = forum_edit_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->forum_edit(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

void Path::open_chat_comment() {
    BOOST_ASSERT(main_widget_);
    long long id = chat_comment_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->chat_comment(tApp->session().load<Comment>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
    t.commit();
}

}

