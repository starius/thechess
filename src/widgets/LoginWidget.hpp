/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

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

class LoginWidget : public Wt::WContainerWidget {
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
