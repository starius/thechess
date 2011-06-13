#ifndef THECHESS_WIDGETS_INTERVAL_WIDGET_HPP_
#define THECHESS_WIDGETS_INTERVAL_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

namespace thechess {
namespace widgets {

class IntervalWidget : public Wt::WTemplate
{
public:
    IntervalWidget(Wt::WWidget* min, Wt::WWidget* max,
        Wt::WContainerWidget* parent=0);
};

}
}

#endif

