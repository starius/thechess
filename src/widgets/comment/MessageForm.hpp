/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_MESSAGE_FORM_HPP_
#define THECHESS_WIDGETS_MESSAGE_FORM_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

class MessageForm : public Wt::WContainerWidget {
public:
    MessageForm(const UserPtr& user, Wt::WContainerWidget* parent = 0);

private:
    UserPtr user_;
    Wt::WFormWidget* message_;
    Wt::WAnchor* message_sent_;

    void send();
};

}

#endif

