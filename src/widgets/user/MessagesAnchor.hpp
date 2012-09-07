/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MESSAGESANCHOR_HPP_
#define THECHESS_WIDGETS_MESSAGESANCHOR_HPP_

#include <Wt/WContainerWidget>

namespace thechess {

/** Anchor which excites when new message is sent */
class MessagesAnchor : public Wt::WContainerWidget {
public:
    /** Constructor */
    MessagesAnchor(Wt::WContainerWidget* parent = 0);
};

}

#endif

