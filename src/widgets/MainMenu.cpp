/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <string>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WAnchor>

#include "Application.hpp"
#include "widgets/MainMenu.hpp"

namespace thechess {

class MainMenuImpl : public Wt::WContainerWidget {
public:
    MainMenuImpl():
        Wt::WContainerWidget() {
        button_("tc.user.Register", "/register/");
        button_("tc.game.Challenge", "/game/challenge/");
        button_("tc.competition.New", "/competition/new/");
        button_("tc.game.List", "/game/");
        button_("tc.competition.List", "/competition/");
        button_("tc.user.Login", "/login/");
        button_("tc.user.Logout", "/logout/");
    }

private:
    void button_(const char* title_id, const std::string path) {
        Wt::WAnchor* button = new Wt::WAnchor(this);
        button->setText(tr(title_id));
        button->setRefInternalPath(path);
        new Wt::WBreak(this);
    }
};

MainMenu::MainMenu(Wt::WContainerWidget* parent) :
    Wt::WCompositeWidget(parent) {
    impl_ = new MainMenuImpl();
    setImplementation(impl_);
}

}

