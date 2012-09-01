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
    type_(type), root_(root), only_ok_(false) {
    dbo::Transaction t(tApp->session());
    setQuery(get_query());
    addColumn("id");
    addColumn("id"); // time
    addColumn("id"); // user
    addColumn("id", ""); // contents
}

static boost::any comment_page(const CommentPtr& o) {
    dbo::Transaction t(tApp->session());
    if (o->type() == Comment::FORUM_TOPIC) {
        return tApp->path().topic_posts()->get_link(o.id());
    } else if (o->type() == Comment::FORUM_POST) {
        return tApp->path().post()->get_link(o.id());
    } else if (o->type() == Comment::FORUM_COMMENT) {
        return tApp->path().post_comment()->get_link(o.id());
    } else if (o->type() == Comment::CHAT_MESSAGE && tApp->user() &&
               tApp->user()->has_permission(User::COMMENTS_REMOVER)) {
        return tApp->path().chat_comment()->get_link(o.id());
    }
    return boost::any();
}

boost::any CommentModel::data(const Wt::WModelIndex& index, int role) const {
    dbo::Transaction t(tApp->session());
    if (role == Wt::DisplayRole) {
        const CommentPtr& o = resultRow(index.row());
        if (index.column() == CONTENTS_COLUMN) {
            return contents(o);
        } else if (index.column() == INIT_COL) {
            return o->init() ?
                   o->init()->safe_username() :
                   Wt::WString::tr("tc.user.Anonymous");
        } else if (index.column() == TIME_COL) {
            if (type() == Comment::CHAT_MESSAGE) {
                return o->created().toString("HH:mm");
            } else {
                return o->created().toString();
            }
        }
    } else if (role == Wt::LinkRole && index.column() == ID_COL) {
        const CommentPtr& o = resultRow(index.row());
        return comment_page(o);
    } else if (role == Wt::LinkRole && index.column() == INIT_COL) {
        const CommentPtr& o = resultRow(index.row());
        UserPtr user = o->init();
        if (user) {
            return tApp->path().user_view()->get_link(user.id());
        }
    } else if (role == Wt::LinkRole && index.column() == TIME_COL) {
        const CommentPtr& o = resultRow(index.row());
        if (type() != Comment::FORUM_POST) {
            return comment_page(o);
        }
    }
    return BaseQM::data(index, role);
}

Wt::WFlags<Wt::ItemFlag> CommentModel::flags(const Wt::WModelIndex& i) const {
    Wt::WFlags<Wt::ItemFlag> f = BaseQM::flags(i);
    if (i.column() == CONTENTS_COLUMN || i.column() == INIT_COL) {
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
    Query result;
    if (root_) {
        result = root_->family().find();
    } else {
        result = tApp->session().find<Comment>();
        if (type_ == Comment::CHAT_MESSAGE) {
            result.where("root_id is null");
        }
    }
    if (type_ != Comment::NO_TYPE) {
        result.where("type = ?").bind(type_);
    } else {
        result.where("type in (?, ?, ?)")
        .bind(Comment::CHAT_MESSAGE)
        .bind(Comment::FORUM_POST)
        .bind(Comment::FORUM_COMMENT);
    }
    if (only_ok_) {
        result.where("state = ?").bind(Comment::OK);
    }
    if (type_ == Comment::FORUM_COMMENT) {
        result.orderBy("show_index");
    } else if (type_ == Comment::NO_TYPE) {
        result.orderBy("id desc");
    }
    return result;
}

void CommentModel::set_only_ok(bool only_ok) {
    only_ok_ = only_ok;
    setQuery(get_query(), /* keep_columns */ true);
}

}

