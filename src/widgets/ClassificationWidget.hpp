#ifndef THECHESS_WIDGETS_CLASSIFICATION_WIDGET_HPP
#define THECHESS_WIDGETS_CLASSIFICATION_WIDGET_HPP

#include <Wt/WComboBox>

#include "model/UserClassification.hpp"

namespace thechess {
namespace widgets {

class ClassificationWidget : public Wt::WComboBox
{
public:
    typedef model::Classification Classification;

    ClassificationWidget(Classification classification=
        model::NO_CLASSIFICATION, Wt::WContainerWidget* parent=0);
    void set_classification(Classification classification);
    Classification get_classification() const;

private:
    Wt::WComboBox* combo_box_;
};

}
}

#endif

