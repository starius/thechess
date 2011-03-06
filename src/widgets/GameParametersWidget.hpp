#ifndef THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_
#define THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_

#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include "model/GameParameters.hpp"

namespace Wt {
    class WContainerWidget;
}

namespace thechess {
namespace widgets {

class GameParametersWidgetImpl;

class GameParametersWidget : public Wt::WCompositeWidget
{
public:
    GameParametersWidget(const model::GameParameters* gp,
        Wt::WContainerWidget* parent=0);

    void apply_parameters(model::GameParameters* gp) const;

private:
    GameParametersWidgetImpl* impl_;
};

}
}


#endif
