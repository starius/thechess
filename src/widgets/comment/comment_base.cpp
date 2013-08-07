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

Wt::WAnchor* global_chat_anchor() {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(Wt::WString::tr("tc.comment.Global_chat"));
    a->setLink(tApp->path().global_chat()->link());
    return a;
}

Wt::WAnchor* game_anchor(int id) {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(Wt::WString::tr("tc.game.Header").arg(id));
    a->setLink(tApp->path().game_view()->get_link(id));
    return a;
}

Wt::WAnchor* competition_anchor(int id) {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(Wt::WString::tr("tc.competition.Header").arg(id));
    a->setLink(tApp->path().competition_view()->get_link(id));
    return a;
}

Wt::WAnchor* team_anchor(int id) {
    Wt::WAnchor* a = new Wt::WAnchor;
    a->setText(Wt::WString::tr("tc.team.Title").arg(id).arg(""));
    a->setLink(tApp->path().team_view()->get_link(id));
    return a;
}

Wt::WAnchor* anchor_to_host(const CommentPtr& comment) {
    dbo::Transaction t(tApp->session());
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
    return 0;
}

}

