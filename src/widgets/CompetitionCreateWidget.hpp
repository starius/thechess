#ifndef THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_

#include <Wt/WContainerWidget>

namespace thechess {
namespace widgets {

class CompetitionParametersWidget2;

class CompetitionCreateWidget : public Wt::WContainerWidget
{
public:
    CompetitionCreateWidget(Wt::WContainerWidget* p=0);

private:
    CompetitionParametersWidget2* cpw_;

    void button_handler_();
};

}
}

#endif

