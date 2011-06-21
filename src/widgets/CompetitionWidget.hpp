#ifndef THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

#include "model/Competition.hpp"

namespace thechess {
namespace widgets {

class CompetitionWidget : public Wt::WTemplate
{
public:
    CompetitionWidget(model::CompetitionPtr competition,
        Wt::WContainerWidget* p=0);

private:
    model::CompetitionPtr c;

    void reprint_();
};

}
}

#endif

