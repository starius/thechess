/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "widgets/comment/CommentModel.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "Application.hpp"

namespace thechess {

CommentModel::CommentModel(Comment::Type type, const CommentPtr& root,
                           Wt::WObject* parent):
    CommentModel::BaseQM(parent),
    type_(type), root_(root) {
    dbo::Transaction t(tApp->session());
    setQuery(get_query());
    addColumn("id");
    addColumn("id"); // time
    addColumn("id"); // user
    addColumn("id", ""); // contents
    t.commit();
}

boost::any CommentModel::data(const Wt::WModelIndex& index, int role) const {
    dbo::Transaction t(tApp->session());
    if (role == Wt::DisplayRole) {
        const CommentPtr& o = resultRow(index.row());
        if (index.column() == CONTENTS_COLUMN) {
            return contents(o);
        } else if (index.column() == INIT_COL) {
            return o->init() ?
                   o->init()->username() :
                   Wt::WString::tr("tc.user.Anonymous");
        } else if (index.column() == TIME_COL) {
            if (type() == Comment::CHAT_MESSAGE) {
                return o->created().toString("HH:mm");
            } else if (type() == Comment::FORUM_POST) {
                return o->created().toString();
            }
        }
    } else if (role == Wt::LinkRole && index.column() == ID_COL) {
        const CommentPtr& o = resultRow(index.row());
        if (type() == Comment::FORUM_TOPIC) {
            return tApp->path().topic_posts()->get_link(o.id());
        } else if (type() == Comment::FORUM_POST) {
            return tApp->path().post()->get_link(o.id());
        } else if (type() == Comment::FORUM_COMMENT) {
            return tApp->path().post_comment()->get_link(o.id());
        }
    } else if (role == Wt::LinkRole && index.column() == INIT_COL) {
        const CommentPtr& o = resultRow(index.row());
        UserPtr user = o->init();
        if (user) {
            return tApp->path().user_view()->get_link(user.id());
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

const char* const COMMENT_INDENT = "<table><tr><td width='{1}'>{2}</td>"
                                   "<td>{3}</td></tr></table>";

Wt::WString CommentModel::contents(const CommentPtr& comment) const {
    if (type() == Comment::FORUM_COMMENT) {
        int depth = comment->depth() - 2;
        int width = depth * 10 + 1;
        std::string spacer(depth, '-');
        return Wt::WString(COMMENT_INDENT)
               .arg(width)
               .arg(spacer)
               .arg(forum_comment_text(comment));
    } else {
        return comment->text_or_removed(tApp->user()); // TODO
    }
}

CommentModel::Query CommentModel::get_query() const {
    if (root_) {
        return root_->family().find().orderBy("show_index");
    } else {
        return tApp->session().find<Comment>().where("type = ?").bind(type_);
    }
}

}

