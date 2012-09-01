/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WText>

#include "widgets/MainPage.hpp"

namespace thechess {

MainPage::MainPage(Wt::WContainerWidget* parent):
    WContainerWidget(parent) {
    addWidget(new Wt::WText(tr("tc.main.main")));
}

}

