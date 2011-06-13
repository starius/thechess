
#include "widgets/IntervalWidget.hpp"

namespace thechess {
namespace widgets {

IntervalWidget:: IntervalWidget(Wt::WWidget* min, Wt::WWidget* max,
    Wt::WContainerWidget* parent):
Wt::WTemplate(tr("thechess.template.interval"), parent)
{
    bindWidget("min", min);
    bindWidget("max", max);
}

}
}

