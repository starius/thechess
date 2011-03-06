#ifndef THECHESS_WIDGETS_PLEASELOGINWIDGET_HPP_
#define THECHESS_WIDGETS_PLEASELOGINWIDGET_HPP_

#include <Wt/WContainerWidget>

namespace thechess {
namespace widgets {

class PleaseLoginWidget : public Wt::WContainerWidget
{
public:
    PleaseLoginWidget(Wt::WContainerWidget* parent=0);
private:
    void login_();
    void register_();
};

}
}


#endif
