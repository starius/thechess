#ifndef THECHESS_WIDGETS_TIMEDELTA_HPP_
#define THECHESS_WIDGETS_TIMEDELTA_HPP_

#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;

#include "time_intervals.hpp"

namespace Wt {
    class WContainerWidget;
    class WFormWidget;
}

namespace thechess {
namespace widgets {

class TimeDeltaWidgetImpl;

class TimeDeltaWidget : public Wt::WCompositeWidget
{
public:
    TimeDeltaWidget(const Td& min, const Td& value, const Td& max,
        Wt::WContainerWidget* parent=0);
    Td timedelta() const;
    Wt::WFormWidget* form_widget(); // for labbels

private:
    TimeDeltaWidgetImpl* impl_;
};

}
}

#endif
