#ifndef THECHESS_WIDGETS_LOGINWIDGET_H_
#define THECHESS_WIDGETS_LOGINWIDGET_H_

#include <Wt/WContainerWidget>

namespace Wt {
    class WPushButton;
    class WLineEdit;
    class WTable;
}

namespace thechess {
namespace widgets {

class LoginWidget : public Wt::WContainerWidget
{
public:
    LoginWidget(Wt::WContainerWidget *parent=0);

private:
    Wt::WTable* table_;
    Wt::WPushButton* button_;
    Wt::WLineEdit* username_;
    Wt::WLineEdit* password_;
    Wt::WText* error_;

    void try_();
};

}
}

#endif
