/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/Header.hpp"

namespace thechess {

Header::Header(const Wt::WString& text, Wt::WContainerWidget* parent):
    Wt::WText(text, Wt::PlainText, parent) {
    setStyleClass("thechess-header");
    setInline(false);
}

}

