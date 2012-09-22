/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>

#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/WString>

#include "widget-set.hpp"
#include "widgets/chess/MovesWidget.hpp"
#include "widgets/game/GameWidget.hpp"
#include "Application.hpp"

namespace thechess {

void init_widget_mode() {
    const Wt::WEnvironment& env = wApp->environment();
    if (!env.getParameter("div")) {
        tApp->quit();
        return;
    }
    if (env.getParameter("type") && *env.getParameter("type") == "game") {
        try {
            if (env.getParameter("game")) {
                int id = boost::lexical_cast<int>(*env.getParameter("game"));
                dbo::Transaction t(tApp->session());
                GamePtr game = tApp->session().load<Game>(id);
                Wt::WContainerWidget* div = new Wt::WContainerWidget;
                div->addWidget(new GameWidget(game));
                Wt::WAnchor* a = new Wt::WAnchor(div);
                a->setText(Wt::WString::tr("tc.game.Header").arg(game.id()));
                a->setLink(tApp->path().game_view()->get_link(game.id()));
                tApp->bindWidget(div, *env.getParameter("div"));
                return;
            }
        } catch (...)
        { }
    }
    tApp->quit();
}

}

