/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMMENT_LIST_HPP_
#define THECHESS_WIDGETS_COMMENT_LIST_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/all.hpp"
#include "widgets/global.hpp"
#include "notify.hpp"

namespace thechess {

/** Widget containing a list of comments */
class CommentList : public Wt::WContainerWidget, public Notifiable {
public:
    /** The type of commenting */
    enum CommentType {
        CHAT, /**< One line messages */
        FORUM /**< Large texts */
    };

    /** Constructor */
    CommentList(const CommentPtr& root, CommentType type,
                Wt::WContainerWidget* parent = 0);

    /** Update widget with new/changed comments */
    virtual void notify(EventPtr);

private:
    class CommentView;

    CommentType type_;
    CommentView* view_;
    Wt::WFormWidget* edit_;

    CommentModel* comment_model() const;

    void add_comment(const CommentPtr& parent);
};

}

#endif

