/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WAnchor>
#include <Wt/WMessageBox>
#include <Wt/WSound>
#include <Wt/Wc/util.hpp>

#include "widgets/user/MessagesAnchor.hpp"
#include "Application.hpp"
#include "model/all.hpp"

namespace thechess {

class MessagesAnchorImpl : public Wt::WAnchor, public Notifiable {
public:
    MessagesAnchorImpl():
        Notifiable(NewMessage(tApp->user().id())),
        box_(0) {
        setText(tr("tc.menu.private_messages"));
        setLink(tApp->path().my_messages()->link());
        clicked().connect(this, &MessagesAnchorImpl::normal_style);
        tApp->path().my_messages()->opened().connect(boost::bind(
                    &MessagesAnchorImpl::normal_style, this));
        normal_style();
        // TODO sound_ = new Wt::WSound("/sound/bark.mp3", this);
        sound_ = new Wt::WSound("/sound/glass.mp3", this);
    }

    ~MessagesAnchorImpl() {
        delete box_;
    }

    void notify(EventPtr e) {
        const NewMessage* new_message = DOWNCAST<const NewMessage*>(e.get());
        int sender_id = new_message->sender_id;
        setStyleClass("thechess-excited");
        Wt::Wc::url::Node* node = tApp->path().parse(wApp->internalPath());
        if (!box_ && node != tApp->path().my_messages() &&
                node != tApp->path().game_view() &&
                (node != tApp->path().user_view() ||
                 tApp->path().user_view()->integer() != sender_id)) {
            box_ = new Wt::WMessageBox;
            box_->setCaption(tr("tc.comment.private_messages"));
            box_->setText(tr("tc.user.New_message"));
            box_->setButtons(Wt::Ok | Wt::Cancel);
            box_->buttonClicked().connect(this,
                                          &MessagesAnchorImpl::box_clicked);
            box_->show();
        }
        sound_->stop();
        sound_->play();
    }

    void box_clicked(Wt::StandardButton button) {
        if (box_) {
            delete box_;
            box_ = 0;
        }
        if (button == Wt::Ok) {
            std::string messages_path = tApp->path().my_messages()->full_path();
            wApp->setInternalPath(messages_path, /* emit */ true);
        }
    }

    void normal_style() {
        setStyleClass("thechess-unexcited");
    }

private:
    Wt::WMessageBox* box_;
    Wt::WSound* sound_;
};

MessagesAnchor::MessagesAnchor(Wt::WContainerWidget* parent) :
    WContainerWidget(parent) {
    addWidget(new MessagesAnchorImpl());
}

}

