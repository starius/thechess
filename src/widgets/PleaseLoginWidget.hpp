/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_PLEASELOGINWIDGET_HPP_
#define THECHESS_WIDGETS_PLEASELOGINWIDGET_HPP_

#include <Wt/WContainerWidget>

namespace thechess {

class PleaseLoginWidget : public Wt::WContainerWidget {
public:
    PleaseLoginWidget(Wt::WContainerWidget* parent = 0);
private:
    void login_();
    void register_();
};

}

#endif

