/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_MAIN_PAGE_HPP_
#define THECHESS_MAIN_PAGE_HPP_

#include <Wt/WGlobal>
#include <Wt/WTemplate>

namespace thechess {

/** Main page widget */
class MainPage : public Wt::WTemplate {
public:
    /** Constructor */
    MainPage(Wt::WContainerWidget* parent = 0);
};

}

#endif

