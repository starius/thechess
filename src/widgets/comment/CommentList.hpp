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

namespace thechess {

/** Widget containing a list of comments */
class CommentList : public Wt::WContainerWidget {
public:
    /** Constructor.
    \see CommentModel::CommentModel
    */
    CommentList(Comment::Type type, const CommentPtr& root = CommentPtr(),
                const UserPtr& init = UserPtr(),
                Wt::WContainerWidget* parent = 0);

    /** Set filter by ip of author.
    Current user must have REGISTRATION_BANNER.
    */
    void set_ip(const std::string ip);

private:
    class CommentView;

    CommentView* view_;
    Wt::WFormWidget* edit_;
    Wt::WTextEdit* post_text_;
    Wt::WCheckBox* only_ok_;
    Wt::WCheckBox* only_my_;
    Wt::WLineEdit* text_like_;

    CommentModel* comment_model() const;

    void print_header();
    void print_post();
    void print_edits();

    void apply();

    void add_comment(const CommentPtr& parent);
};

}

#endif

