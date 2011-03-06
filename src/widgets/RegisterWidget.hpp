#ifndef THECHESS_WIDGETS_REGISTERWIDGET_H_
#define THECHESS_WIDGETS_REGISTERWIDGET_H_

#include <Wt/WContainerWidget>

namespace Wt {
    class WPushButton;
    class WLineEdit;
    class WTable;
}

namespace thechess {
namespace widgets {

class RegisterWidget : public Wt::WContainerWidget
{
public:
    RegisterWidget(Wt::WContainerWidget *parent=0);

private:
    Wt::WTable* table_;
    Wt::WPushButton* button_;
    Wt::WLineEdit* username_;
    Wt::WLineEdit* password_;
    Wt::WText* username_error_;
    Wt::WText* password_error_;

    void try_register_();
};

}
}

#endif
