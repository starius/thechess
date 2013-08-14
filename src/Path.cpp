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
    random_competitor_ = new PredefinedNode("random", this);
    all_sessions_ = new PredefinedNode("sessions", user_list_);
    user_view_ = new IntegerNode(user_list_);
    user_challenge_ = new PredefinedNode("challenge", user_view_);
    virtuals_of_user_ = new PredefinedNode("virtuals", user_view_);
    ip_of_user_ = new PredefinedNode("ip", user_view_);
    games_of_user_ = new PredefinedNode("games", user_view_);
    competitions_of_user_ = new PredefinedNode("competitions", user_view_);
    all_virtuals_ = new PredefinedNode("virtuals", user_list_);
    settings_page_ = new PredefinedNode("settings", user_list_);
    game_list_ = new PredefinedNode("game", this);
    game_view_ = new IntegerNode(game_list_);
    PredefinedNode* game_moves = new PredefinedNode("moves", game_view_);
    game_move_view_ = new IntegerNode(game_moves);
    game_new_ = new PredefinedNode("new", game_list_);
    gp_list_ = new PredefinedNode("parameters", game_list_);
    gp_view_ = new IntegerNode(gp_list_);
    gp_challenge_ = new PredefinedNode("challenge", gp_view_);
    gp_new_ = new PredefinedNode("new", gp_list_);
    competition_list_ = new PredefinedNode("competition", this);
    competition_view_ = new IntegerNode(competition_list_);
    games_of_competition_ = new PredefinedNode("games", competition_view_);
    virtuals_in_competition_ = new PredefinedNode("virtuals",
            competition_view_);
    competition_new_ = new PredefinedNode("new", competition_list_);
    cp_list_ = new PredefinedNode("parameters", competition_list_);
    cp_view_ = new IntegerNode(cp_list_);
    cp_new_ = new PredefinedNode("new", cp_list_);
    cp_challenge_ = new PredefinedNode("challenge", cp_view_);
    board_root_ = new PredefinedNode("board", this);
    board_ = new StringNode(board_root_);
    board_games_ = new PredefinedNode("games", board_);
    moves_root_ = new PredefinedNode("moves", this);
    moves_ = new StringNode(moves_root_);
    moves_challenge_ = new PredefinedNode("challenge", moves_);
    moves_games_ = new PredefinedNode("games", moves_);
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
    PredefinedNode* admin = new PredefinedNode("admin", this);
    admin_log_ = new PredefinedNode("log", admin);
    all_banned_ip_ = new PredefinedNode("ip", user_list_);
    PredefinedNode* edit = new PredefinedNode("edit", all_banned_ip_);
    banned_ip_ = new StringNode(all_banned_ip_);
    edit_banned_ip_ = new IntegerNode(edit);
    new_ip_ban_ = new StringNode(ip_of_user_);
    all_comments_ = new PredefinedNode("comments", this);
    my_messages_ = new PredefinedNode("messages", this);
    user_comments_ = new PredefinedNode("comments", user_view_);
    global_chat_ = new PredefinedNode("chat", this);
    teams_list_ = new PredefinedNode("team", this);
    team_view_ = new IntegerNode(teams_list_);
    team_competitions_ = new PredefinedNode("competitions", team_view_);
    if (wApp) {
        wApp->internalPathChanged().connect(this, &Parser::open);
    }
}

void Path::connect_main_widget(MainWidget* mw) {
    main_widget_ = mw;
    connect(this, boost::bind(&MainWidget::main_page, mw));
    connect(user_list_, boost::bind(&MainWidget::user_list, mw));
    connect(random_competitor_,
            boost::bind(&Path::open_random_competitor, this));
    connect(all_sessions_, boost::bind(&MainWidget::all_sessions, mw));
    connect(user_view_, boost::bind(&Path::open_user, this));
    connect(user_challenge_, boost::bind(&Path::open_user_challenge, this));
    connect(virtuals_of_user_, boost::bind(&Path::open_virtuals_of_user, this));
    connect(ip_of_user_, boost::bind(&Path::open_ip_of_user, this));
    connect(games_of_user_, boost::bind(&Path::open_games_of_user, this));
    connect(competitions_of_user_, boost::bind(&Path::open_competitions_of_user,
            this));
    //connect(all_virtuals_, boost::bind(&MainWidget::all_virtuals, mw));
    connect(settings_page_, boost::bind(&MainWidget::settings_page, mw));
    connect(game_list_, boost::bind(&MainWidget::game_list, mw));
    connect(game_view_, boost::bind(&Path::open_game, this));
    connect(game_move_view_, boost::bind(&Path::open_game_move, this));
    connect(game_new_, boost::bind(&MainWidget::game_new, mw));
    connect(gp_list_, boost::bind(&MainWidget::gp_list, mw));
    connect(gp_view_, boost::bind(&Path::open_gp, this));
    connect(gp_challenge_, boost::bind(&Path::open_gp_challenge, this));
    connect(competition_list_, boost::bind(&MainWidget::competition_list, mw));
    connect(competition_view_, boost::bind(&Path::open_competition, this));
    connect(games_of_competition_,
            boost::bind(&Path::open_games_of_competition, this));
    connect(virtuals_in_competition_,
            boost::bind(&Path::open_virtuals_in_competition, this));
    connect(competition_new_, boost::bind(&MainWidget::competition_new, mw));
    connect(cp_view_, boost::bind(&Path::open_cp, this));
    connect(cp_challenge_, boost::bind(&Path::open_cp_challenge, this));
    connect(board_, boost::bind(&Path::open_board, this));
    connect(board_games_, boost::bind(&Path::open_board_games, this));
    connect(moves_, boost::bind(&Path::open_moves, this));
    connect(moves_challenge_, boost::bind(&Path::open_moves_challenge, this));
    connect(moves_games_, boost::bind(&Path::open_moves_games, this));
    connect(board_root_, boost::bind(&MainWidget::moves_widget, mw));
    connect(moves_root_, boost::bind(&MainWidget::moves_widget, mw));
    connect(topics_, boost::bind(&MainWidget::forum_topics, mw));
    connect(topic_posts_, boost::bind(&Path::open_forum_topic_posts, this));
    connect(all_posts_, boost::bind(&MainWidget::forum_all_posts, mw));
    connect(post_, boost::bind(&Path::open_forum_post, this));
    connect(post_comment_, boost::bind(&Path::open_forum_post_comment, this));
    connect(forum_edit_, boost::bind(&Path::open_forum_edit, this));
    connect(chat_comment_, boost::bind(&Path::open_chat_comment, this));
    connect(admin_log_, boost::bind(&MainWidget::admin_log, mw));
    connect(all_banned_ip_, boost::bind(&MainWidget::all_banned_ip, mw));
    connect(banned_ip_, boost::bind(&Path::open_banned_ip, this));
    connect(edit_banned_ip_, boost::bind(&Path::open_edit_banned_ip, this));
    connect(new_ip_ban_, boost::bind(&Path::open_new_ip_ban, this));
    connect(all_comments_, boost::bind(&MainWidget::all_comments, mw));
    connect(my_messages_, boost::bind(&MainWidget::my_messages, mw));
    connect(user_comments_, boost::bind(&Path::open_user_comments, this));
    connect(global_chat_, boost::bind(&MainWidget::global_chat, mw));
    connect(teams_list_, boost::bind(&MainWidget::teams_list, mw));
    connect(team_view_, boost::bind(&Path::open_team, this));
    connect(team_competitions_,
            boost::bind(&Path::open_team_competitions, this));
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

void Path::open_random_competitor() {
    BOOST_ASSERT(main_widget_);
    dbo::Transaction t(tApp->session());
    if (tApp->user()) {
        try {
            UserPtr user = tApp->session().find<User>()
                           .where("rights != ?").bind(NONE)
                           .where("vacation_until is null")
                           .where("last_online >= ?")
                           .bind(now() - Options::instance()->away_timeout())
                           .where("id <> ?").bind(tApp->user())
                           .orderBy("random()").limit(1);
            main_widget_->user_challenge(user);
        } catch (...) {
            main_widget_->game_new();
        }
    }
}

void Path::open_user_challenge() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->user_challenge(tApp->session().load<User>(id));
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

void Path::open_ip_of_user() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->ip_of_user(tApp->session().load<User>(id));
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

void Path::open_game_move() {
    BOOST_ASSERT(main_widget_);
    long long id = game_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        int move_n = game_move_view_->integer();
        main_widget_->game_view(tApp->session().load<Game>(id), move_n);
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_gp() {
    BOOST_ASSERT(main_widget_);
    long long id = gp_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->gp_view(tApp->session().load<GP>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_gp_challenge() {
    BOOST_ASSERT(main_widget_);
    long long id = gp_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->gp_challenge(tApp->session().load<GP>(id));
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

void Path::open_games_of_competition() {
    BOOST_ASSERT(main_widget_);
    long long id = competition_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        CompetitionPtr c = tApp->session().load<Competition>(id);
        main_widget_->games_of_competition(c);
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_virtuals_in_competition() {
    BOOST_ASSERT(main_widget_);
    long long id = competition_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        CompetitionPtr c = tApp->session().load<Competition>(id);
        main_widget_->virtuals_in_competition(c);
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_cp() {
    BOOST_ASSERT(main_widget_);
    long long id = cp_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        CPPtr cp = tApp->session().load<CP>(id);
        main_widget_->cp_view(cp);
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_cp_challenge() {
    BOOST_ASSERT(main_widget_);
    long long id = cp_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        CPPtr cp = tApp->session().load<CP>(id);
        main_widget_->cp_challenge(cp);
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_board() {
    main_widget_->board_view(board_->string());
}

void Path::open_board_games() {
    main_widget_->board_games(board_->string());
}

void Path::open_moves() {
    main_widget_->moves_view(moves_->string());
}

void Path::open_moves_challenge() {
    main_widget_->moves_challenge(moves_->string());
}

void Path::open_moves_games() {
    main_widget_->moves_games(moves_->string());
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

void Path::open_banned_ip() {
    BOOST_ASSERT(main_widget_);
    main_widget_->banned_ip(banned_ip_->string());
}

void Path::open_edit_banned_ip() {
    BOOST_ASSERT(main_widget_);
    long long id = edit_banned_ip_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->edit_banned_ip(tApp->session().load<IpBan>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_new_ip_ban() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        UserPtr user = tApp->session().load<User>(id);
        main_widget_->new_ip_ban(new_ip_ban_->string(), user);
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_user_comments() {
    BOOST_ASSERT(main_widget_);
    long long id = user_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->user_comments(tApp->session().load<User>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_team() {
    BOOST_ASSERT(main_widget_);
    long long id = team_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->team_view(tApp->session().load<Team>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

void Path::open_team_competitions() {
    BOOST_ASSERT(main_widget_);
    long long id = team_view_->integer();
    dbo::Transaction t(tApp->session());
    try {
        main_widget_->team_competitions(tApp->session().load<Team>(id));
    } catch (dbo::ObjectNotFoundException)
    { }
}

}

