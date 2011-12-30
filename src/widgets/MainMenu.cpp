/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <string>

#include <Wt/WBreak>
#include <Wt/WAnchor>

#include "widgets/MainMenu.hpp"
#include "Path.hpp"

namespace thechess {

class MainMenu::MainMenuImpl : public Wt::WContainerWidget {
public:
    MainMenuImpl(Path* path):
        Wt::WContainerWidget() {
        button_("tc.user.Register", path->register_form());
        button_("tc.game.Challenge", path->game_new());
        button_("tc.competition.New", path->competition_new());
        button_("tc.game.List", path->game_list());
        button_("tc.competition.List", path->competition_list());
        button_("tc.user.Login", path->login_form());
        // FIXME logout
    }

private:
    void button_(const char* title_id, Wt::Wc::url::Node* node) {
        Wt::WAnchor* button = new Wt::WAnchor(this);
        button->setText(tr(title_id));
        button->setRefInternalPath(node->full_path());
        new Wt::WBreak(this);
    }
};

MainMenu::MainMenu(Path* path, Wt::WContainerWidget* parent) :
    Wt::WCompositeWidget(parent) {
    impl_ = new MainMenuImpl(path);
    setImplementation(impl_);
}

}

