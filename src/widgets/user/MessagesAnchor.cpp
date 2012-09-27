/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WAnchor>

#include "widgets/user/MessagesAnchor.hpp"
#include "Application.hpp"
#include "model/all.hpp"

namespace thechess {

class MessagesAnchorImpl : public Wt::WAnchor, public Notifiable {
public:
    MessagesAnchorImpl():
        Notifiable(NewMessage(tApp->user().id())) {
        setText(tr("tc.menu.private_messages"));
        setLink(tApp->path().my_messages()->link());
        clicked().connect(this, &MessagesAnchorImpl::normal_style);
        normal_style();
    }

    void notify(EventPtr) {
        setStyleClass("thechess-excited");
    }

    void normal_style() {
        setStyleClass("thechess-unexcited");
    }
};

MessagesAnchor::MessagesAnchor(Wt::WContainerWidget* parent) :
    WContainerWidget(parent) {
    addWidget(new MessagesAnchorImpl());
}

}

