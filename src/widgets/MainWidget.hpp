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

#include "thechess-global.hpp"
#include "model/all.hpp"

namespace thechess {

/** Main Application widget */
class MainWidget : public Wt::WContainerWidget {
public:
    /** Constructor */
    MainWidget(Wt::WContainerWidget* parent = 0);

    /** Show the menu with references */
    void show_menu(Path* path);

    /** List of users */
    void user_list();

    /** View the user */
    void user_view(const UserPtr& user);

    /** List of games */
    void game_list();

    /** View the game */
    void game_view(const GamePtr& game);

    /** Create new game */
    void game_new();

    /** List of competitions */
    void competition_list();

    /** View the competition */
    void competition_view(const CompetitionPtr& competition);

    /** Create new competition */
    void competition_new();

    /** Get authentication widget.
    \attention Auth widget should be set (by Application).
    */
    Wt::Auth::AuthWidget* auth_widget() {
        return auth_widget_;
    }

    /** Set authentication widget */
    void set_auth_widget(Wt::Auth::AuthWidget* widget);

private:
    Wt::WContainerWidget* auth_widget_container_;
    Wt::Auth::AuthWidget* auth_widget_;

    Wt::WBorderLayout* l();

    Wt::WContainerWidget* contents();

    void set_contents(WWidget* widget);
};

}

#endif

