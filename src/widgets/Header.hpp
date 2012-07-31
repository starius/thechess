/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_HAEDER_HPP_
#define THECHESS_WIDGETS_HAEDER_HPP_

#include <Wt/WGlobal>
#include <Wt/WText>

namespace thechess {

/** A header */
class Header : public Wt::WText {
public:
    /** Constructor */
    Header(const Wt::WString& text, Wt::WContainerWidget* parent = 0);
};

}

#endif

