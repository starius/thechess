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
#include "Path.hpp"

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
        clear();
        add_section("tc.game.game");
        add_button("tc.game.List", path_->game_list());
        if (user_items_) {
            add_button("tc.game.Challenge", path_->game_new());
        }
        add_section("tc.competition.competition");
        add_button("tc.competition.List", path_->competition_list());
        if (user_items_) {
            add_button("tc.competition.New", path_->competition_new());
        }
        if (user_items_) {
            add_section("tc.user.user");
            add_button("tc.user.settings", path_->settings_page());
        }
        add_section("tc.forum.forum");
        add_button("tc.forum.topics", path_->topics());
        add_button("tc.forum.all_posts", path_->all_posts());
    }

    void add_button(const char* title_id, Wt::Wc::url::Node* node) {
        Wt::WContainerWidget* c = new Wt::WContainerWidget(this);
        Wt::WAnchor* button = new Wt::WAnchor(c);
        button->setText(tr(title_id));
        button->setLink(node->link());
    }

    void add_section(const char* title_id) {
        Wt::WContainerWidget* c = new Wt::WContainerWidget(this);
        Wt::WText* text = new Wt::WText(tr(title_id), c);
        text->setStyleClass("thechess-section");
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

