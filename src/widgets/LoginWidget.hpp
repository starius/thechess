/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_LOGINWIDGET_H_
#define THECHESS_WIDGETS_LOGINWIDGET_H_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

namespace thechess {

class LoginWidget : public Wt::WContainerWidget {
public:
    LoginWidget(Wt::WContainerWidget* parent = 0);

private:
    Wt::WTable* table_;
    Wt::WPushButton* button_;
    Wt::WLineEdit* username_;
    Wt::WLineEdit* password_;
    Wt::WText* error_;

    void try_();
};

}

#endif

