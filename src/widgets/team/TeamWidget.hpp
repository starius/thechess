/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_TEAM_WIDGET_HPP_
#define THECHESS_WIDGETS_TEAM_WIDGET_HPP_

#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

class TeamWidget : public Wt::WContainerWidget, public Notifiable {
public:
    TeamWidget(const TeamPtr& team);

    void notify(EventPtr);

private:
    TeamPtr team_;

    typedef boost::function<void(const UserPtr&, const TeamPtr&)> UserAction;

    void reprint();
    void print_title();
    void print_members();
    void print_candidates();
    void print_banned();
    void print_manager();
    void print_chat();
    void list_users(const Users& users, const Wt::WString& header);
    void user_buttons(const UserPtr& user, Wt::WContainerWidget* item);
    void add_button(const Wt::WString& button, const UserAction& user_action,
                    Wt::WContainerWidget* item);
    void apply_action(const UserAction& user_action);
    void show_edit();
};

}

#endif

