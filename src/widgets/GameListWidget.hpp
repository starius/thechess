#ifndef THECHESS_WIDGETS_GAMELISTWIDGET_HPP_
#define THECHESS_WIDGETS_GAMELISTWIDGET_HPP_

#include <Wt/WCompositeWidget>

namespace Wt {
    class WContainerWidget;
}

namespace thechess {
namespace widgets {

class GameListWidgetImpl;

class GameListWidget : public Wt::WCompositeWidget
{
public:
    GameListWidget(Wt::WContainerWidget* parent=0);
private:
    GameListWidgetImpl* impl_;
};

}
}

#endif
