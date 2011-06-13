
#include "widgets/ClassificationWidget.hpp"
#include "model/UserClassification.hpp"

namespace thechess {
namespace widgets {
using namespace model;

ClassificationWidget::ClassificationWidget(Classification min,
    Classification value, Classification max, Wt::WContainerWidget* parent):
Wt::WComboBox(parent), first_i_(-1)
{
    for (int i = 0; i < classifications_size; i++)
    {
        Classification classification = classifications[i];
        if (min <= classification && classification <= max)
        {
            addItem(classification2str(classification));
            if (classification == value)
            {
                setCurrentIndex(i);
            }
            if (first_i_ == -1)
            {
                first_i_ = i;
            }
        }
    }
}

Classification ClassificationWidget::value() const
{
    return classifications[currentIndex() + first_i_];
}

}
}

