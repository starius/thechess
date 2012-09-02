/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_FOOTER_HPP_
#define THECHESS_FOOTER_HPP_

#include <Wt/WContainerWidget>

namespace thechess {

/** Footer of main widget */
class Footer : public Wt::WContainerWidget {
public:
    /** Constructor */
    Footer(Wt::WContainerWidget* parent = 0);
};

}

#endif

