/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <algorithm>

#include "widgets/comment/CommentModel.hpp"
#include "widgets/comment/forum_comment_text.hpp"
#include "widgets/comment/comment_base.hpp"
#include "Application.hpp"

namespace thechess {

CommentModel::CommentModel(Comment::Type type, const CommentPtr& root,
                           const UserPtr& init, Wt::WObject* parent):
    CommentModel::BaseQM(parent),
    type_(type), root_(root), init_(init) {
    dbo::Transaction t(tApp->session());
    if (type == Comment::PRIVATE_MESSAGE && tApp->user()) {
        tApp->user().reread();
        if (!tApp->user()->has_comment_base()) {
            tApp->user().modify()->comment_base();
        }
        // set root for Notifiable, to update on new messages
        root_ =  tApp->user()->comment_base();
        if (init_) {
            // other user (private chat)
            init_.reread();
            if (!init_->has_comment_base()) {
                init_.modify()->comment_base();
            }
        }
    }
    only_ok_ = User::has_s(SWITCH_ONLY_OK_COMMENTS);
    only_draft_ = User::has_s(SWITCH_ONLY_DRAFT_COMMENTS);
    only_my_ = User::has_s(SWITCH_ONLY_MY_COMMENTS);
    setQuery(get_query());
    addColumn("id");
    addColumn("id"); // time
    addColumn("id"); // user
    addColumn("id"); // destination
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
    } else if (o->type() == Comment::CHAT_MESSAGE) {
        return tApp->path().chat_comment()->get_link(o.id());
    } else if (o->type() == Comment::PRIVATE_MESSAGE) {
        return tApp->path().chat_comment()->get_link(o.id());
    }
    return boost::any();
}

UserPtr destination_user(const CommentPtr& message) {
    dbo::Transaction t(tApp->session());
    try {
        UserPtr user = tApp->session().find<User>()
                       .where("comment_base_id = ?").bind(message->root());
        if (user) {
            return user;
        }
    } catch (...)
    { }
    return UserPtr();
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
        } else if (index.column() == DESTINATION_COL &&
                   type() == Comment::PRIVATE_MESSAGE) {
            UserPtr user = destination_user(o);
            if (user) {
                return user->safe_username();
            }
        } else if (index.column() == TIME_COL) {
            if (now() - o->created() < DAY) {
                return time2str(o->created(), "HH:mm");
            } else if (now() - o->created() < 365 * DAY) {
                return time2str(o->created(), "dd.MM");
            } else {
                return time2str(o->created(), "yyyy");
            }
        } else if (index.column() == ID_COL && type() == Comment::NO_TYPE) {
            return host_text(o);
        }
    } else if (role == Wt::ToolTipRole && index.column() == TIME_COL) {
        const CommentPtr& o = resultRow(index.row());
        return time2str(o->created(), "dd MMM yyyy hh:mm");
    } else if (role == Wt::LinkRole) {
        const CommentPtr& o = resultRow(index.row());
        if (index.column() == ID_COL) {
            if (type() == Comment::NO_TYPE) {
                return host_link(o);
            } else {
                return comment_page(o);
            }
        } else if (index.column() == CONTENTS_COLUMN &&
                   type() == Comment::FORUM_TOPIC) {
            return comment_page(o);
        } else if (index.column() == INIT_COL) {
            UserPtr user = o->init();
            if (user) {
                return tApp->path().user_view()->get_link(user.id());
            }
        } else if (index.column() == DESTINATION_COL &&
                   type() == Comment::PRIVATE_MESSAGE) {
            UserPtr user = destination_user(o);
            if (user) {
                return tApp->path().user_view()->get_link(user.id());
            }
        } else if (index.column() == TIME_COL) {
            if (type() != Comment::FORUM_POST) {
                return comment_page(o);
            }
        }
    }
    return BaseQM::data(index, role);
}

Wt::WFlags<Wt::ItemFlag> CommentModel::flags(const Wt::WModelIndex& i) const {
    Wt::WFlags<Wt::ItemFlag> f = BaseQM::flags(i);
    if (i.column() == CONTENTS_COLUMN || i.column() == INIT_COL
            || i.column() == DESTINATION_COL) {
        f |= Wt::ItemIsXHTMLText;
    }
    return f;
}

const char* const COMMENT_INDENT = "<table><tr><td width='{1}'>{2}</td>"
                                   "<td>{3}</td></tr></table>";

Wt::WString CommentModel::contents(const CommentPtr& comment) const {
    if (type() == Comment::FORUM_COMMENT) {
        int depth = std::max(0, comment->depth() - 2);
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
    if (type_ == Comment::PRIVATE_MESSAGE) {
        result = tApp->session().find<Comment>();
        const UserPtr& me = tApp->user();
        const CommentPtr& my_base = me->comment_base();
        if (init_) {
            // private chat with user init_
            const UserPtr& he = init_;
            CommentPtr his_base = he->comment_base();
            result.where("root_id in (?, ?) and init_id in (?, ?)")
            .bind(his_base).bind(my_base).bind(he).bind(me);
        } else {
            // all my private messages with all users
            result.where("root_id = ? or init_id = ?")
            .bind(my_base).bind(me);
        }
    } else if (root_) {
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
    if (init_ && type_ != Comment::PRIVATE_MESSAGE) {
        result.where("init_id = ?").bind(init_);
    }
    if (only_my_) {
        result.where("init_id = ?").bind(tApp->user().id());
    }
    if (only_ok_) {
        result.where("state = ?").bind(Comment::OK);
    } else if (only_draft_) {
        result.where("state = ?").bind(Comment::DRAFT);
    }
    if (!ip_.empty()) {
        result.where("ip = ?").bind(ip_);
    }
    if (!text_like_.empty()) {
        result.where("(text like ? or id = ?)");
        result.bind("%" + text_like_ + "%");
        result.bind(Wt::Wc::str2int(text_like_.toUTF8()));
    }
    if (type_ == Comment::FORUM_COMMENT) {
        result.orderBy("show_index");
    } else {
        result.orderBy("id");
    }
    return result;
}

void CommentModel::set_only_ok(bool only_ok) {
    if (only_ok != only_ok_) {
        only_ok_ = only_ok;
        User::set_s(SWITCH_ONLY_OK_COMMENTS, only_ok);
        setQuery(get_query(), /* keep_columns */ true);
    }
}

void CommentModel::set_only_draft(bool only_draft) {
    if (only_draft != only_draft_) {
        only_draft_ = only_draft;
        User::set_s(SWITCH_ONLY_DRAFT_COMMENTS, only_draft);
        setQuery(get_query(), /* keep_columns */ true);
    }
}

void CommentModel::set_only_my(bool only_my) {
    if (only_my != only_my_) {
        only_my_ = only_my;
        User::set_s(SWITCH_ONLY_MY_COMMENTS, only_my);
        setQuery(get_query(), /* keep_columns */ true);
    }
}

void CommentModel::set_ip(const std::string ip) {
    if (ip != ip_) {
        ip_ = ip;
        setQuery(get_query(), /* keep_columns */ true);
    }
}

void CommentModel::set_text_like(const Wt::WString& text_like) {
    if (text_like != text_like_) {
        text_like_ = text_like;
        setQuery(get_query(), /* keep_columns */ true);
    }
}

}

