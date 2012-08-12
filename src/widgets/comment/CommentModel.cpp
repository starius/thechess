/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Utils>

#include "widgets/comment/CommentModel.hpp"
#include "Application.hpp"

namespace thechess {

CommentModel::CommentModel(Comment::Type type, const CommentPtr& root,
                           Wt::WObject* parent):
    CommentModel::BaseQM(parent),
    type_(type), root_(root) {
    dbo::Transaction t(tApp->session());
    setQuery(get_query());
    addColumn("id");
    addColumn("id", ""); // contents
    t.commit();
}

boost::any CommentModel::data(const Wt::WModelIndex& index, int role) const {
    dbo::Transaction t(tApp->session());
    if (role == Wt::DisplayRole && index.column() == CONTENTS_COLUMN) {
        const CommentPtr& o = resultRow(index.row());
        return contents(o);
    } else if (role == Wt::LinkRole && index.column() == ID_COL) {
        const CommentPtr& o = resultRow(index.row());
        if (type() == Comment::FORUM_TOPIC) {
            return tApp->path().topic_posts()->get_link(o.id());
        } else if (type() == Comment::FORUM_POST) {
            return tApp->path().post()->get_link(o.id());
        }
    }
    return BaseQM::data(index, role);
}

Wt::WFlags<Wt::ItemFlag> CommentModel::flags(const Wt::WModelIndex& i) const {
    Wt::WFlags<Wt::ItemFlag> f = BaseQM::flags(i);
    if (i.column() == CONTENTS_COLUMN) {
        f |= Wt::ItemIsXHTMLText;
    }
    return f;
}

Wt::WString CommentModel::contents(const CommentPtr& comment) const {
    if (type() == Comment::FORUM_COMMENT) {
        Wt::WString text = comment->text();
        Wt::Utils::removeScript(text);
        return Wt::WString::tr("tc.forum.comment_template")
               .arg(comment->created().toString())
               .arg(comment->init()->username())
               .arg(tApp->path().post_comment()->get_full_path(comment.id()))
               .arg(text);
    } else {
        return comment->text(); // TODO
    }
}

CommentModel::Query CommentModel::get_query() const {
    if (root_) {
        return root_->family().find();
    } else {
        return tApp->session().find<Comment>().where("type = ?").bind(type_);
    }
}

}

