#ifndef THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_WIDGET_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTemplate>

#include "model/Competition.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
namespace widgets {

class CompetitionWidget : public Wt::WTemplate, public Notifiable
{
public:
    CompetitionWidget(model::CompetitionPtr competition,
        Wt::WContainerWidget* p=0);

    virtual void notify();

private:
    model::CompetitionPtr c;

    void reprint_();
};

}
}

#endif

