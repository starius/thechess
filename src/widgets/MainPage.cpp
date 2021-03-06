/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WBreak>
#include <Wt/Wc/util.hpp>

#include "widgets/MainPage.hpp"
#include "widgets/user/user_anchor.hpp"
#include "Options.hpp"
#include "Application.hpp"

namespace thechess {

MainPage::MainPage(Wt::WContainerWidget* parent):
    WTemplate(parent) {
    dbo::Transaction t(tApp->session());
    refresh();
    int best_players_shown = Options::instance()->best_players_shown();
    Users best_users = tApp->session().find<User>()
                       .where("rights <> 0")
                       .where("vacation_until is null")
                       .where("last_online > ?").bind(now() - 10 * WEEK)
                       .orderBy("games_stat_elo desc")
                       .limit(best_players_shown);
    Wt::WContainerWidget* best_players = new Wt::WContainerWidget();
    BOOST_FOREACH (UserPtr user, best_users) {
        best_players->addWidget(user_anchor(user));
        best_players->addWidget(new Wt::WBreak());
    }
    bindWidget("best_players", best_players);
    int champion_id = Options::instance()->champion_id();
    bool champion_bound = false;
    if (champion_id > 0) {
        try {
            UserPtr champion = tApp->session().load<User>(champion_id);
            bindWidget("champion", user_anchor(champion));
            champion_bound = true;
        } catch (...)
        { }
    }
    if (!champion_bound) {
        bindString("champion", "");
    }
    Users moders_q = tApp->session().find<User>().where("admin_rights <> 0");
    Wt::WContainerWidget* moders = new Wt::WContainerWidget();
    BOOST_FOREACH (UserPtr user, moders_q) {
        moders->addWidget(user_anchor(user));
        moders->addWidget(new Wt::WBreak());
    }
    bindWidget("moders", moders);
    bindInt("all_sessions", Application::sessions_number());
    int online = tApp->session().find<User>()
                 .where("last_online >= ?")
                 .bind(now() - Options::instance()->away_timeout())
                 .resultList().size();
    bindInt("online", online);
}

void MainPage::refresh() {
    dbo::Transaction t(tApp->session());
    int comment_id = Options::instance()->main_page_content_id();
    if (comment_id > 0) {
        try {
            CommentPtr comment = tApp->session().load<Comment>(comment_id,
                                 /* forceReread */ true);
            if (comment->type() == Comment::FORUM_COMMENT ||
                    comment->type() == Comment::FORUM_POST_TEXT) {
                setTemplateText(comment->text());
            }
        } catch (...)
        { }
    }
    if (templateText().empty()) {
        setTemplateText(tr("tc.main.main_template"));
    }
}

}

