#ifndef THECHESS_WIDGETS_MAINMENU_HPP_
#define THECHESS_WIDGETS_MAINMENU_HPP_

#include <Wt/WCompositeWidget>

namespace Wt {
    class WContainerWidget;
}

namespace thechess {
namespace widgets {

class MainMenuImpl;

class MainMenu : public Wt::WCompositeWidget
{
public:
    MainMenu(Wt::WContainerWidget* parent=0);
private:
    MainMenuImpl* impl_;
};

}
}

#endif
