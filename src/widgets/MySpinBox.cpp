#include <Wt/WContainerWidget>

#include "widgets/MySpinBox.hpp"

namespace thechess {
namespace widgets {

MySpinBox::MySpinBox(Wt::WContainerWidget* parent) :
Wt::WSpinBox(parent)
{
}

double MySpinBox::value() const
{
    double v = Wt::WSpinBox::value();
    if (v < minimum())
    {
        v = minimum();
    }
    if (v > maximum())
    {
        v = maximum();
    }
    return v;
}

}
}
