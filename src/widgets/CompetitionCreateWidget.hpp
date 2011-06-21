#ifndef THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_CREATE_WIDGET_HPP_

#include <Wt/WContainerWidget>

namespace Wt {
    class WLineEdit;
    class WTextEdit;
}

namespace thechess {
namespace widgets {

class CompetitionParametersWidget;

class CompetitionCreateWidget : public Wt::WContainerWidget
{
public:
    CompetitionCreateWidget(Wt::WContainerWidget* p=0);

private:
    CompetitionParametersWidget* cpw_;
    Wt::WLineEdit* name_;
    Wt::WTextEdit* description_;

    void button_handler_();
};

}
}

#endif

