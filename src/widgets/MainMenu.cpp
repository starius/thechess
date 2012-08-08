/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <string>

#include <Wt/WAnchor>

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
        if (user_items_) {
            button_("tc.game.Challenge", path_->game_new());
            button_("tc.competition.New", path_->competition_new());
        }
        button_("tc.game.List", path_->game_list());
        button_("tc.competition.List", path_->competition_list());
        if (user_items_) {
            button_("Wt.Auth.updatepassword", path_->update_password());
        }
    }

    void button_(const char* title_id, Wt::Wc::url::Node* node) {
        Wt::WContainerWidget* c = new Wt::WContainerWidget(this);
        Wt::WAnchor* button = new Wt::WAnchor(c);
        button->setText(tr(title_id));
        button->setLink(node->link());
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

