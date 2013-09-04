/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WAnchor>

#include "widgets/comment/comment_base.hpp"
#include "Application.hpp"

namespace thechess {

Object comment_base(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment && comment->type() == Comment::CHAT_MESSAGE &&
            comment->root()) {
        return comment_base(comment->root());
    } else if (!comment || comment->type() != Comment::CHAT_ROOT) {
        return Object(NOEVENT, 0);
    }
    try {
        GamePtr game = tApp->session().find<Game>()
                       .where("comment_base_id = ?").bind(comment);
        if (game) {
            return Object(GAME, int(game.id()));
        }
    } catch (...)
    { }
    try {
        CompetitionPtr c = tApp->session().find<Competition>()
                           .where("comment_base_id = ?").bind(comment);
        if (c) {
            return Object(COMPETITION, int(c.id()));
        }
    } catch (...)
    { }
    try {
        TeamPtr team = tApp->session().find<Team>()
                       .where("comment_base_id = ?").bind(comment);
        if (team) {
            return Object(TEAM_OBJECT, int(team.id()));
        }
    } catch (...)
    { }
    return Object(NOEVENT, 0);
}

#define TR(x) Wt::WString::tr(x)

Wt::WString global_chat_text() {
    return TR("tc.comment.Global_chat");
}

Wt::WLink global_chat_link() {
    return tApp->path().global_chat()->link();
}

Wt::WAnchor* global_chat_anchor() {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(global_chat_text());
    a->setLink(global_chat_link());
    return a;
}

Wt::WString game_text(int id) {
    return TR("tc.game.Header").arg(id);
}

Wt::WLink game_link(int id) {
    return tApp->path().game_view()->get_link(id);
}

Wt::WAnchor* game_anchor(int id) {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(game_text(id));
    a->setLink(game_link(id));
    return a;
}

Wt::WString competition_text(int id) {
    dbo::Transaction t(tApp->session());
    Wt::WString title(Wt::WString::tr("tc.competition.Header"));
    CompetitionPtr c = tApp->session().load<Competition>(id);
    if (c->name().empty()) {
        title.arg(id);
    } else {
        title.arg(c->name());
    }
    return title;
}

Wt::WLink competition_link(int id) {
    return tApp->path().competition_view()->get_link(id);
}

Wt::WAnchor* competition_anchor(int id) {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(competition_text(id));
    a->setLink(competition_link(id));
    return a;
}

Wt::WString team_text(int id) {
    dbo::Transaction t(tApp->session());
    Wt::WString title(Wt::WString::tr("tc.team.Title").arg(id));
    TeamPtr team = tApp->session().load<Team>(id);
    if (!team->name().empty()) {
        title.arg(team->name());
    } else {
        title.arg("");
    }
    return title;
}

Wt::WLink team_link(int id) {
    return tApp->path().team_view()->get_link(id);
}

Wt::WAnchor* team_anchor(int id) {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(team_text(id));
    a->setLink(team_link(id));
    return a;
}

Wt::WString host_text(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() == Comment::CHAT_MESSAGE) {
        if (!comment->root()) {
            return global_chat_text();
        }
        Object object = comment_base(comment);
        if (object.id == 0) {
            return "";
        }
        if (object.type == GAME) {
            return game_text(object.id);
        } else if (object.type == COMPETITION) {
            return competition_text(object.id);
        } else if (object.type == TEAM_OBJECT) {
            return team_text(object.id);
        }
    } else if (comment->type() == Comment::FORUM_TOPIC) {
        return Wt::WString::tr("tc.forum.topics");
    } else if (comment->type() == Comment::FORUM_POST) {
        CommentPtr topic = comment->root();
        if (topic) {
            return topic->text();
        } else {
            return Wt::WString::tr("tc.forum.all_posts");
        }
    } else if (comment->type() == Comment::FORUM_COMMENT) {
        CommentPtr post_text = comment->root();
        CommentPtr post = post_text->parent();
        return post->text();
    }
    return "";
}

Wt::WLink host_link(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() == Comment::CHAT_MESSAGE) {
        if (!comment->root()) {
            return global_chat_link();
        }
        Object object = comment_base(comment);
        if (object.id == 0) {
            return Wt::WLink();
        }
        if (object.type == GAME) {
            return game_link(object.id);
        } else if (object.type == COMPETITION) {
            return competition_link(object.id);
        } else if (object.type == TEAM_OBJECT) {
            return team_link(object.id);
        }
    } else if (comment->type() == Comment::FORUM_TOPIC) {
        return tApp->path().topics()->link();
    } else if (comment->type() == Comment::FORUM_POST) {
        CommentPtr topic = comment->root();
        if (topic) {
            return tApp->path().topic_posts()->get_link(topic.id());
        } else {
            return tApp->path().all_posts()->link();
        }
    } else if (comment->type() == Comment::FORUM_COMMENT) {
        CommentPtr post_text = comment->root();
        CommentPtr post = post_text->parent();
        return tApp->path().post()->get_link(post.id());
    }
    return Wt::WLink();
}

Wt::WAnchor* anchor_to_host(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
    if (comment->type() == Comment::CHAT_MESSAGE) {
        // this special case to avoid double call to comment_base()
        if (comment->type() == Comment::CHAT_MESSAGE && !comment->root()) {
            return global_chat_anchor();
        }
        Object object = comment_base(comment);
        if (object.id == 0) {
            return 0;
        }
        if (object.type == GAME) {
            return game_anchor(object.id);
        } else if (object.type == COMPETITION) {
            return competition_anchor(object.id);
        } else if (object.type == TEAM_OBJECT) {
            return team_anchor(object.id);
        }
    } else {
        Wt::WAnchor* a = new Wt::WAnchor;
        a->setText(host_text(comment));
        a->setLink(host_link(comment));
        return a;
    }
}

}

