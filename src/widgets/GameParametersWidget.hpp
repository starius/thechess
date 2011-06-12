#ifndef THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_
#define THECHESS_WIDGETS_GAMAPARAMETERSWIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WCheckBox>
#include <Wt/WSpinBox>

#include "model/GameParameters.hpp"
#include "widgets/MovesWidget.hpp"
#include "widgets/TimeDeltaWidget.hpp"

namespace thechess {
namespace widgets {

class GameParametersWidget : public Wt::WContainerWidget
{
public:
    GameParametersWidget(const model::GameParameters* gp,
        Wt::WContainerWidget* parent=0);
    void apply_parameters(model::GameParameters* gp);

private:
    MovesWidget* moves_widget_;

    TimeDeltaWidget* limit_std_;
    TimeDeltaWidget* limit_private_init_;

    Wt::WCheckBox* norating_;

    TimeDeltaWidget* pause_limit_init_;
    Wt::WSpinBox* first_draw_;

    int first_draw_value_();
};

}
}


#endif

