
#include "widgets/ClassificationWidget.hpp"
#include "model/UserClassification.hpp"

namespace thechess {
namespace widgets {

using namespace model;

ClassificationWidget::ClassificationWidget(Classification classification,
    Wt::WContainerWidget* parent):
Wt::WComboBox(parent)
{
    for (int i = 0; i < classifications_size; i++)
    {
        combo_box_->addItem(classification2str(classifications[i]));
    }
    set_classification(classification);
}

void ClassificationWidget::set_classification(Classification classification)
{
    for (int i = 0; i < classifications_size; i++)
    {
        if (classifications[i] == classification)
        {
            combo_box_->setCurrentIndex(i);
        }
    }
}

Classification ClassificationWidget::get_classification() const
{
    return classifications[combo_box_->currentIndex()];
}

}
}

