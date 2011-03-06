#include <string>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WPushButton>

#include "ThechessApplication.hpp"
#include "widgets/RegisterWidget.hpp"
#include "widgets/LoginWidget.hpp"
#include "widgets/GameCreateWidget.hpp"
#include "widgets/GameListWidget.hpp"
#include "widgets/MainMenu.hpp"

namespace thechess {
namespace widgets {

class MainMenuImpl : public Wt::WContainerWidget
{
public:
    MainMenuImpl():
    Wt::WContainerWidget()
    {
        button_<RegisterWidget>("thechess.register", "/register/");
        button_<GameCreateWidget>("thechess.challenge", "/game/challenge/");
        button_<GameListWidget>("thechess.game_list", "/game/");
        button_<LoginWidget>("thechess.login", "/login/");
        button_<&ThechessApplication::logout>("thechess.logout");
   }

private:
    Wt::WPushButton* empty_button(const char* title_id)
    {
        return new Wt::WPushButton(Wt::WString::tr(title_id), this);
    }

    typedef void (ThechessApplication::* TCMethod)();

    template <TCMethod method>
    void button_(const char* title_id)
    {
        Wt::WPushButton* button = empty_button(title_id);
        button->clicked().connect(tApp, method);
        new Wt::WBreak(this);
    }

    template <typename W>
    void button_(const char* title_id, const std::string path)
    {
        Wt::WPushButton* button = empty_button(title_id);
        button->clicked().connect(boost::bind(
            &ThechessApplication::show_<W>, tApp, path));
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

