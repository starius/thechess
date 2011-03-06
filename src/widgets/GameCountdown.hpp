#ifndef THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_
#define THECHESS_WIDGETS_GAMECOUNTDOWN_HPP_

#include <Wt/WViewWidget>

#include "model/Game.hpp"

namespace Wt {
    class WContainerWidget;
}

namespace thechess {

namespace widgets {

using model::GamePtr;

class GameCountdown : public Wt::WViewWidget
{
public:
    GameCountdown(GamePtr game, Wt::WContainerWidget* parent=0);
private:
    GamePtr game_;

    virtual Wt::WWidget* renderView();
};

}
}

#endif
