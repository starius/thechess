/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WString>
#include <Wt/Utils>
#include <Wt/Wc/util.hpp>

#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

void admin_log(const Wt::WString& message) {
    dbo::Transaction t(tApp->session());
    CommentPtr comment = tApp->session().add(new Comment(true));
    comment.modify()->set_type(Comment::LOG_ENTRY);
    comment.modify()->set_init(tApp->user());
    comment.modify()->set_text(message);
    comment.modify()->set_state(Comment::OK);
}

Wt::WString html_a(const Wt::WString& path, const Wt::WString& text) {
    return "<a href='" + path + "'>" + text + "</a>";
}

Wt::WString html_a(Wt::Wc::url::IntegerNode* node, int id,
                   const Wt::WString& text) {
    return html_a(node->get_full_path(id), text);
}

Wt::WString user_a(int user_id) {
    dbo::Transaction t(tApp->session());
    UserPtr u = tApp->session().load<User>(user_id);
    Wt::WString name = Wt::Utils::htmlEncode(u->username());
    return html_a(tApp->path().user_view(), user_id, name);
}

Wt::WString comp_a(int id) {
    Wt::WString t = Wt::WString("competition {1}").arg(id);
    return html_a(tApp->path().competition_view(), id, t);
}

Wt::WString comm_a(int comment_id) {
    dbo::Transaction t(tApp->session());
    CommentPtr comment = tApp->session().load<Comment>(comment_id);
    Wt::Wc::url::IntegerNode* node = 0;
    Wt::WString text;
    int id;
    if (comment->type() == Comment::CHAT_MESSAGE) {
        node = tApp->path().chat_comment();
        text = "chat message {1}";
        id = comment.id();
    } else if (comment->type() == Comment::FORUM_POST) {
        node = tApp->path().post();
        text = "forum post {1}";
        id = comment.id();
    } else if (comment->type() == Comment::FORUM_POST_TEXT) {
        node = tApp->path().post();
        text = "forum post {1}";
        id = comment->parent().id();
    } else if (comment->type() == Comment::FORUM_TOPIC) {
        node = tApp->path().topic_posts();
        text = "forum topic {1}";
        id = comment.id();
    } else if (comment->type() == Comment::FORUM_COMMENT) {
        node = tApp->path().post_comment();
        text = "forum comment {1}";
        id = comment.id();
    }
    if (node) {
        text.arg(id);
        return html_a(node, id, text);
    }
    return TO_S(comment_id);
}

Wt::WString game_a(int id) {
    Wt::WString t = Wt::WString("game {1}").arg(id);
    return html_a(tApp->path().game_view(), id, t);
}

}

