#ifndef THECHESS_WIDGETS_MYSPINBOX
#define THECHESS_WIDGETS_MYSPINBOX

#include <Wt/WSpinBox>

namespace Wt {
    class WContainerWidget;
}

namespace thechess {
namespace widgets {

class MySpinBox : public Wt::WSpinBox
{
public:
    MySpinBox(Wt::WContainerWidget* parent=0);
    double value() const;
};

}
}

#endif
