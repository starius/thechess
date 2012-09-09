/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <string>

#include <Wt/WAnchor>
#include <Wt/WText>

#include "widgets/MainMenu.hpp"
#include "widgets/user/MessagesAnchor.hpp"
#include "Path.hpp"
#include "Application.hpp"

namespace thechess {

class MainMenu::MainMenuImpl : public Wt::WContainerWidget {
public:
    MainMenuImpl(Path* path):
        Wt::WContainerWidget(),
        user_items_(false),
        path_(path) {
        setList(true);
        reprint();
    }

    void show_user_items(bool show) {
        user_items_ = show;
        reprint();
    }

private:
    bool user_items_;
    Path* path_;

    void reprint() {
        dbo::Transaction t(tApp->session());
        clear();
        add_section("tc.menu.game");
        add_button("tc.menu.games_list", path_->game_list());
        if (user_items_) {
            add_button("tc.menu.challenge", path_->game_new());
        }
        add_button("tc.menu.testing_board", path_->board_root());
        add_button("tc.menu.game_parameters", path_->gp_list());
        add_section("tc.menu.competition");
        add_button("tc.menu.competitions_list", path_->competition_list());
        if (user_items_) {
            add_button("tc.menu.new_competition", path_->competition_new());
        }
        add_section("tc.menu.user");
        add_button("tc.menu.users_list", path_->user_list());
        if (user_items_) {
            add_button("tc.menu.settings", path_->settings_page());
            add_button("tc.menu.my_page",
                       path_->user_view()->get_link(tApp->user().id()));
            addWidget(new MessagesAnchor());
            if (tApp->user()->has_permission(VIRTUALS_VIEWER)) {
                add_button("tc.menu.virtuals_list", path_->all_virtuals());
            }
            if (tApp->user()->has_permission(LOGS_READER)) {
                add_button("tc.menu.admin_log", path_->admin_log());
            }
            if (tApp->user()->has_permission(REGISTRATION_BANNER)) {
                add_button("tc.menu.banned_ip", path_->all_banned_ip());
            }
        }
        add_section("tc.menu.forum");
        add_button("tc.menu.topics", path_->topics());
        add_button("tc.menu.all_posts", path_->all_posts());
        add_button("tc.menu.all_comments", path_->all_comments());
        add_button("tc.menu.global_chat", path_->global_chat());
    }

    void add_button(const char* title_id, const Wt::WLink& link) {
        Wt::WContainerWidget* c = new Wt::WContainerWidget(this);
        Wt::WAnchor* button = new Wt::WAnchor(c);
        button->setText(tr(title_id));
        button->setLink(link);
    }

    void add_button(const char* title_id, Wt::Wc::url::Node* node) {
        add_button(title_id, node->link());
    }

    void add_section(const char* title_id) {
        Wt::WContainerWidget* c = new Wt::WContainerWidget(this);
        Wt::WText* text = new Wt::WText(tr(title_id), c);
        text->setStyleClass("thechess-section");
        doJavaScript("$(" + c->jsRef() + ").css('list-style-type', 'none');");
    }
};

MainMenu::MainMenu(Path* path, Wt::WContainerWidget* parent) :
    Wt::WCompositeWidget(parent) {
    impl_ = new MainMenuImpl(path);
    setImplementation(impl_);
}

void MainMenu::show_user_items(bool show) {
    impl_->show_user_items(show);
}

}

