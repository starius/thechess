#include <Wt/WContainerWidget>

#include "widgets/MySpinBox.hpp"

namespace thechess {
namespace widgets {

MySpinBox::MySpinBox(Wt::WContainerWidget* parent) :
Wt::WSpinBox(parent)
{
}

int MySpinBox::value() const
{
    int v = Wt::WSpinBox::value();
    if (v < minimum())
    {
        v = minimum();
    }
    else if (v > maximum())
    {
        v = maximum();
    }
    return v;
}

MyDoubleSpinBox::MyDoubleSpinBox(Wt::WContainerWidget* parent) :
Wt::WDoubleSpinBox(parent)
{
}

double MyDoubleSpinBox::value() const
{
    double v = Wt::WDoubleSpinBox::value();
    if (v < minimum())
    {
        v = minimum();
    }
    else if (v > maximum())
    {
        v = maximum();
    }
    return v;
}

}
}

