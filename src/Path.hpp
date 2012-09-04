/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_PATH_HPP_
#define THECHESS_PATH_HPP_

#include <Wt/WGlobal>
#include <Wt/Wc/Url.hpp>

#include "widgets/global.hpp"

namespace thechess {

namespace url = Wt::Wc::url;

/** Url processing.
This class uses tApp to listen to url changes and to get Session instance.
*/
class Path : public Wt::Wc::url::Parser {
public:
    /** Constructor */
    Path(Wt::WObject* parent = 0);

    /** Connect nodes to slots of main widget */
    void connect_main_widget(MainWidget* main_widget);

    /** Main page */
    url::Parser* main_page() {
        return this;
    }

    /** List of users */
    url::PredefinedNode* user_list() const {
        return user_list_;
    }

    /** View the user */
    url::IntegerNode* user_view() const {
        return user_view_;
    }

    /** View virtuals of user */
    url::PredefinedNode* virtuals_of_user() const {
        return virtuals_of_user_;
    }

    /** View games of user */
    url::PredefinedNode* games_of_user() const {
        return games_of_user_;
    }

    /** View competitions of user */
    url::PredefinedNode* competitions_of_user() const {
        return competitions_of_user_;
    }

    /** View all virtuals */
    url::PredefinedNode* all_virtuals() const {
        return all_virtuals_;
    }

    /** Settings page */
    url::PredefinedNode* settings_page() const {
        return settings_page_;
    }

    /** List of games */
    url::PredefinedNode* game_list() const {
        return game_list_;
    }

    /** View the game */
    url::IntegerNode* game_view() const {
        return game_view_;
    }

    /** Create new game */
    url::PredefinedNode* game_new() const {
        return game_new_;
    }

    /** List of GPs */
    url::PredefinedNode* gp_list() const {
        return gp_list_;
    }

    /** View the GP */
    url::IntegerNode* gp_view() const {
        return gp_view_;
    }

    /** Create new GP */
    url::PredefinedNode* gp_new() const {
        return gp_new_;
    }

    /** List of competitions */
    url::PredefinedNode* competition_list() const {
        return competition_list_;
    }

    /** View the competition */
    url::IntegerNode* competition_view() const {
        return competition_view_;
    }

    /** Create new competition */
    url::PredefinedNode* competition_new() const {
        return competition_new_;
    }

    /** List of CPs */
    url::PredefinedNode* cp_list() const {
        return cp_list_;
    }

    /** View the CP */
    url::IntegerNode* cp_view() const {
        return cp_view_;
    }

    /** Create new CP */
    url::PredefinedNode* cp_new() const {
        return cp_new_;
    }

    /** Root of board URL's */
    url::PredefinedNode* board_root() const {
        return board_root_;
    }

    /** Board URL.
    Parameter: Board::to_string()
    */
    url::StringNode* board() const {
        return board_;
    }

    /** Forum topics */
    url::PredefinedNode* topics() const {
        return topics_;
    }

    /** List of posts of the topic */
    url::IntegerNode* topic_posts() const {
        return topic_posts_;
    }

    /** List of all posts */
    url::PredefinedNode* all_posts() const {
        return all_posts_;
    }

    /** Page of the post */
    url::IntegerNode* post() const {
        return post_;
    }

    /** Page of the post comment */
    url::IntegerNode* post_comment() const {
        return post_comment_;
    }

    /** Page of the post edit widget */
    url::IntegerNode* forum_edit() const {
        return forum_edit_;
    }

    /** Page of comment  */
    url::IntegerNode* chat_comment() const {
        return chat_comment_;
    }

    /** Admin log */
    url::PredefinedNode* admin_log() const {
        return admin_log_;
    }

    /** All bans by IP */
    url::PredefinedNode* all_banned_ip() const {
        return all_banned_ip_;
    }

    /** Bans of this IP */
    url::StringNode* banned_ip() const {
        return banned_ip_;
    }

    /** All comments */
    url::PredefinedNode* all_comments() const {
        return all_comments_;
    }

    /** Global chat */
    url::PredefinedNode* global_chat() const {
        return global_chat_;
    }

private:
    MainWidget* main_widget_;
    url::PredefinedNode* user_list_;
    url::IntegerNode* user_view_;
    url::PredefinedNode* virtuals_of_user_;
    url::PredefinedNode* games_of_user_;
    url::PredefinedNode* competitions_of_user_;
    url::PredefinedNode* all_virtuals_;
    url::PredefinedNode* settings_page_;
    url::PredefinedNode* game_list_;
    url::IntegerNode* game_view_;
    url::PredefinedNode* game_new_;
    url::PredefinedNode* gp_list_;
    url::IntegerNode* gp_view_;
    url::PredefinedNode* gp_new_;
    url::PredefinedNode* competition_list_;
    url::IntegerNode* competition_view_;
    url::PredefinedNode* competition_new_;
    url::PredefinedNode* cp_list_;
    url::IntegerNode* cp_view_;
    url::PredefinedNode* cp_new_;
    url::PredefinedNode* board_root_;
    url::StringNode* board_;
    url::PredefinedNode* topics_;
    url::IntegerNode* topic_posts_;
    url::PredefinedNode* all_posts_;
    url::IntegerNode* post_;
    url::IntegerNode* post_comment_;
    url::IntegerNode* forum_edit_;
    url::IntegerNode* chat_comment_;
    url::PredefinedNode* admin_log_;
    url::PredefinedNode* all_banned_ip_;
    url::StringNode* banned_ip_;
    url::PredefinedNode* all_comments_;
    url::PredefinedNode* global_chat_;

    void open_user();
    void open_virtuals_of_user();
    void open_games_of_user();
    void open_competitions_of_user();
    void open_game();
    void open_gp();
    void open_competition();
    void open_board();
    void open_forum_topic_posts();
    void open_forum_post();
    void open_forum_post_comment();
    void open_forum_edit();
    void open_chat_comment();
    void open_banned_ip();
};

}

#endif

