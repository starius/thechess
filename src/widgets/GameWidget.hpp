#ifndef THECHESS_WIDGETS_GAMEWIDGET_HPP_
#define THECHESS_WIDGETS_GAMEWIDGET_HPP_

#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include "model/Game.hpp"

namespace Wt {
    class WContainerWidget;
}

namespace thechess {
namespace widgets {

using model::GamePtr;

class GameWidgetImpl;

class GameWidget : public Wt::WCompositeWidget
{
public:
    GameWidget(GamePtr game, Wt::WContainerWidget* parent=0);
private:
    GameWidgetImpl* impl_;
};

}
}

#endif
