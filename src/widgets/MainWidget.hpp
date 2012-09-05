/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MAIN_WIDGET_HPP_
#define THECHESS_MAIN_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/Wc/global.hpp>

#include "thechess-global.hpp"
#include "model/global.hpp"
#include "widgets/global.hpp"

namespace thechess {

/** Main Application widget */
class MainWidget : public Wt::WContainerWidget {
public:
    /** Constructor */
    MainWidget(Wt::WContainerWidget* parent = 0);

    /** Show the menu with references */
    void show_menu(Path* path);

    /** Get main menu */
    MainMenu* main_menu();

    /** Update list of user's games (East) */
    void update_my_games();

    /** Show main page */
    void main_page();

    /** List of users */
    void user_list();

    /** View the user */
    void user_view(const UserPtr& user);

    /** View user's virtuals */
    void virtuals_of_user(const UserPtr& user);

    /** View user's games */
    void games_of_user(const UserPtr& user);

    /** View user's competitions */
    void competitions_of_user(const UserPtr& user);

    /** View all virtuals */
    void all_virtuals();

    /** Settings page */
    void settings_page();

    /** Create update-password dialog */
    void update_password();

    /** List of games */
    void game_list();

    /** View the game */
    void game_view(const GamePtr& game);

    /** Create new game */
    void game_new();

    /** List of game parameters */
    void gp_list();

    /** View the gp */
    void gp_view(const GPPtr& gp);

    /** List of competitions */
    void competition_list();

    /** View the competition */
    void competition_view(const CompetitionPtr& competition);

    /** Create new competition */
    void competition_new();

    /** View the board */
    void board_view(const std::string& data);

    /** View MovesWidget */
    void moves_widget();

    /** View forum topics */
    void forum_topics();

    /** View posts of this topic */
    void forum_topic_posts(const CommentPtr& topic);

    /** View all posts */
    void forum_all_posts();

    /** View post */
    void forum_post(const CommentPtr& post);

    /** View post comment */
    void forum_post_comment(const CommentPtr& comment);

    /** Edit forum post */
    void forum_edit(const CommentPtr& post_or_text);

    /** View chat comment */
    void chat_comment(const CommentPtr& comment);

    /** View admin log */
    void admin_log();

    /** View all banned IPs */
    void all_banned_ip();

    /** View bans of the IP */
    void banned_ip(const std::string& ip);

    /** New IP ban of user */
    void new_ip_ban(const std::string& ip, const UserPtr& user);

    /** View all comments */
    void all_comments();

    /** View comments of the user */
    void user_comments(const UserPtr& user);

    /** View global chat */
    void global_chat();

    /** Get authentication widget.
    \attention Auth widget should be set (by Application).
    */
    Wt::Auth::AuthWidget* auth_widget();

    /** Set authentication widget */
    void set_auth_widget();

    /** Get SWF store.
    \attention The widget must be set (by Application).
    */
    Wt::Wc::SWFStore* swf_store();

    /** Set swf store.
    Ownership of the swf store is transferred.
    Do not call this method twice.
    */
    void set_swfstore(Wt::Wc::SWFStore* swfstore);

    /** Set if top block (right to AuthWidget) is shown.
    This block (TopBlock) is shown if user is logged in.
    */
    void set_top_block_shown(bool shown = true);

private:
    Wt::WContainerWidget* auth_place_;
    Wt::WContainerWidget* top_place_;
    Wt::WContainerWidget* menu_place_;
    Wt::WContainerWidget* contents_place_;
    Wt::WContainerWidget* mygames_place_;
    Wt::WContainerWidget* bottom_place_;

    void set_contents(WWidget* widget);
    void show_countup();
    void show_is_banned();
};

}

#endif

