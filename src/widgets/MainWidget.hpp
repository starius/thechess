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

#include "model/all.hpp"

namespace thechess {

class Path;

/** Main Application widget */
class MainWidget : public Wt::WContainerWidget {
public:
    /** Constructor */
    MainWidget(Wt::WContainerWidget* parent = 0);

    /** Show the menu with references */
    void show_menu(Path* path);

    /** Login form */
    void login_form();

    /** Register form */
    void register_form();

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

private:
    Wt::WBorderLayout* l();

    Wt::WContainerWidget* contents();

    void set_contents(WWidget* widget);
};

}

#endif

