/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include <Wt/WLogger>
#include <Wt/WText>
#include <Wt/WTemplate>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/WDateTime>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "chess/Piece.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"
#include "ThechessApplication.hpp"
#include "widgets/GameCountdown.hpp"

namespace thechess {

class SingleTimeout : public Wt::WTemplate {
public:
    SingleTimeout(const Wt::WString& name, bool active,
                  const Td& limit_private_now, const Td& limit_std_now,
                  Wt::WContainerWidget* parent = 0) :
        Wt::WTemplate(tr("tc.game.countdown_template"), parent) {
        bindString("name", name);
        Wt::WText* limit_private_text =
            new Wt::WText(td2str(limit_private_now));
        bindWidget("limit_private", limit_private_text);
        Wt::WText* limit_std_text =
            new Wt::WText(td2str(limit_std_now));
        bindWidget("limit_std", limit_std_text);
        if (active) {
            doJavaScript(str(boost::format(
                                 "thechess_countdown_start('%s', %d/1000, '%s', %d/1000);")
                             % limit_private_text->id()
                             % limit_private_now.total_milliseconds()
                             % limit_std_text->id()
                             % limit_std_now.total_milliseconds()));
        }
    }

};

class GameCountdownImpl : public Wt::WContainerWidget {
public:
    GameCountdownImpl(GamePtr game, Wt::WContainerWidget* parent = 0) :
        Wt::WContainerWidget(parent), game_(game) {
        dbo::Transaction t(tApp->session());
        UserPtr user1;
        UserPtr user2;
        UserPtr me = tApp->user();
        if (game_->is_member(me)) {
            user1 = me;
            user2 = game_->other_user(me);
        } else if (game_->white() && game_->black()) {
            user1 = game_->white();
            user2 = game_->black();
        } else if (game_->init()) {
            user1 = game_->init();
        }
        if (user1) {
            std::cout << "single_countdown_(user1);" << std::endl;
            single_countdown_(user1);
        }
        if (user2) {
            single_countdown_(user2);
        }
        t.commit();
    }

private:
    GamePtr game_;

    void single_countdown_(UserPtr user) {
        const Wt::WString& name = user->username();
        bool active = game_->state() == Game::ACTIVE
                      && user == game_->order_user();
        Td limit_private_now = game_->limit_private_now(user);
        Td limit_std_now = game_->limit_std_now(user);
        new SingleTimeout(name, active, limit_private_now,
                          limit_std_now, this);
    }

};

GameCountdown::GameCountdown(GamePtr game,
                             Wt::WContainerWidget* parent) :
    Wt::WViewWidget(parent), game_(game) {
    wApp->require("js/jquery.countdown.pack.js");
    wApp->require("js/countdown.js");
}

Wt::WWidget* GameCountdown::renderView() {
    try {
        return new GameCountdownImpl(game_);
    } catch (std::exception& e) {
        tApp->log("warning") << e.what();
    }
    return new Wt::WText();
}

}

