/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <cmath>
#include <algorithm>

#include <Wt/WText>
#include <Wt/WImage>
#include <Wt/WContainerWidget>
#include <Wt/Wc/util.hpp>

#include "widgets/user/awards.hpp"
#include "Application.hpp"

namespace thechess {

double Log2(double n) {
    return log(n) / log(2.);
}

Awards awards_of(const UserPtr& user) {
    dbo::Transaction t(tApp->session());
    Awards result;
    const EloPlayer& comp_stat = user->competitions_stat();
    int won_comp = comp_stat.wins() + comp_stat.draws();
    result.competitions = won_comp ? int(Log2(won_comp)) + 1 : 0;
    const EloPlayer& e = user->games_stat();
    result.game_3 = std::max(e.all() / 500, e.wins() / 250);
    result.game_2 = result.game_3 ? 0 :
                    (e.all() >= 250 || e.wins() >= 125) ? 1 : 0;
    result.game_1 = (result.game_3 || result.game_2) ? 0 :
                    (e.all() >= 100, e.wins() >= 50) ? 1 : 0;
    return result;
}

static void add_award(Wt::WContainerWidget* c, int awards, const char* path) {
    if (awards) {
        if (awards >= 2) {
            c->addWidget(new Wt::WText(TO_S(awards)));
        }
        Wt::WImage* im = new Wt::WImage(path);
        im->setHeight(60);
        c->addWidget(im);
    }
}

/** Return widget with awards */
Wt::WWidget* awards_widget(const UserPtr& user) {
    Wt::WContainerWidget* result = new Wt::WContainerWidget();
    Awards awards = awards_of(user);
    add_award(result, awards.competitions, "img/awards/competition.png");
    add_award(result, awards.game_1, "img/awards/game-1.gif");
    add_award(result, awards.game_2, "img/awards/game-2.gif");
    add_award(result, awards.game_3, "img/awards/game-3.gif");
    return result;
}

}

