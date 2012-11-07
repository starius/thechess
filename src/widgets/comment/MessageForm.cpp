/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WPushButton>
#include <Wt/WLineEdit>
#include <Wt/WTextEdit>
#include <Wt/WAnchor>
#include <Wt/WBreak>
#include <Wt/WText>

#include "widgets/comment/MessageForm.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"

namespace thechess {

MessageForm::MessageForm(const UserPtr& user, Wt::WContainerWidget* parent):
    WContainerWidget(parent),
    user_(user) {
    if (tApp->user() && tApp->user()->can_send_message(user_)) {
        new Wt::WBreak(this);
        new Header(tr("tc.comment.private_messages"), this);
        if (!User::has_s(SWITCH_FORMATTING_CHAT)) {
            Wt::WLineEdit* line_edit = new Wt::WLineEdit(this);
            line_edit->setTextSize(80);
            message_ = line_edit;
            message_->enterPressed().connect(this, &MessageForm::send);
        } else {
            Wt::WTextEdit* text_edit = new Wt::WTextEdit(this);
            patch_text_edit(text_edit);
            new Wt::WBreak(this);
            message_ = text_edit;
        }
        Wt::WPushButton* b;
        b = new Wt::WPushButton(tr("tc.common.Send"), this);
        b->clicked().connect(this, &MessageForm::send);
        message_sent_ = new Wt::WAnchor(this);
        message_sent_->setText(tr("tc.user.Message_sent"));
        message_sent_->setLink(tApp->path().my_messages()->link());
        message_sent_->hide();
    }
}

void MessageForm::send() {
    tApp->user().reread();
    dbo::Transaction t(tApp->session());
    if (tApp->user() && tApp->user()->can_send_message(user_)) {
        User::send_message(tApp->user(), user_, message_->valueText());
        message_->setValueText("");
        message_sent_->show();
    }
}

}

