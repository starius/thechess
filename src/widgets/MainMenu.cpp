/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <string>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WAnchor>

#include "ThechessApplication.hpp"
#include "widgets/MainMenu.hpp"

namespace thechess {
namespace widgets {

class MainMenuImpl : public Wt::WContainerWidget
{
public:
    MainMenuImpl():
    Wt::WContainerWidget()
    {
        button_("thechess.register", "/register/");
        button_("thechess.challenge", "/game/challenge/");
        button_("thechess.competition.new", "/competition/new/");
        button_("thechess.game_list", "/game/");
        button_("thechess.competition_list", "/competition/");
        button_("thechess.login", "/login/");
        button_("thechess.logout", "/logout/");
    }

private:
    void button_(const char* title_id, const std::string path)
    {
        Wt::WAnchor* button = new Wt::WAnchor(this);
        button->setText(tr(title_id));
        button->setRefInternalPath(path);
        new Wt::WBreak(this);
    }
};

MainMenu::MainMenu(Wt::WContainerWidget* parent) :
Wt::WCompositeWidget(parent)
{
    impl_ = new MainMenuImpl();
    setImplementation(impl_);
}

}
}

