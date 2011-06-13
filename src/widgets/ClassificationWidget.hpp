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

    ClassificationWidget(Classification min=model::NO_CLASSIFICATION,
        Classification value=model::NO_CLASSIFICATION,
        Classification max=model::SUPER_GRANDMASTER,
        Wt::WContainerWidget* parent=0);
    Classification value() const;

private:
    int first_i_;
};

}
}

#endif

