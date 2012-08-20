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

#include "model/global.hpp"
#include "widgets/global.hpp"
#include "notify.hpp"

namespace thechess {

/** Widget containing a list of comments */
class CommentList : public Wt::WContainerWidget, public Notifiable {
public:
    /** Constructor */
    CommentList(Comment::Type type, const CommentPtr& root = CommentPtr(),
                Wt::WContainerWidget* parent = 0);

    /** Update widget with new/changed comments */
    virtual void notify(EventPtr);

private:
    class CommentView;

    CommentView* view_;
    Wt::WFormWidget* edit_;
    Wt::WTextEdit* post_text_;

    CommentModel* comment_model() const;

    void print_header();
    void print_post();
    void print_edits();

    void add_comment(const CommentPtr& parent);
};

}

#endif

