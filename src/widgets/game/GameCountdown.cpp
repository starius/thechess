/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
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
#include <Wt/Wc/Countdown.hpp>

#include "chess/Piece.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "widgets/game/GameCountdown.hpp"

namespace thechess {

class SingleTimeout : public Wt::WTemplate {
public:
    SingleTimeout(const Wt::WString& name, bool active,
                  const Td& limit_private_now, const Td& limit_std_now,
                  Wt::WContainerWidget* parent = 0) :
        Wt::WTemplate(tr("tc.game.countdown_template"), parent) {
        using namespace Wt::Wc;
        bindString("name", name);
        Countdown* limit_private = new Countdown();
        limit_private->pause();
        limit_private->set_until(limit_private_now);
        bindWidget("limit_private", limit_private);
        Countdown* limit_std = new Countdown();
        limit_std->change("alwaysExpire", "true");
        limit_std->pause();
        limit_std->set_until(limit_std_now);
        bindWidget("limit_std", limit_std);
        if (active) {
            limit_std->resume();
            limit_private->resume(limit_std_now);
        }
    }

};

class GameCountdownImpl : public Wt::WContainerWidget {
public:
    GameCountdownImpl(const GamePtr& game, Wt::WContainerWidget* parent = 0) :
        Wt::WContainerWidget(parent), game_(game) {
        dbo::Transaction t(tApp->session());
        UserPtr user1;
        UserPtr user2;
        const UserPtr& me = tApp->user();
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

    void single_countdown_(const UserPtr& user) {
        const Wt::WString& name = user->username();
        bool active = game_->state() == Game::ACTIVE
                      && user == game_->order_user();
        Td limit_private_now = game_->limit_private_now(user);
        Td limit_std_now = game_->limit_std_now(user);
        new SingleTimeout(name, active, limit_private_now,
                          limit_std_now, this);
    }

};

GameCountdown::GameCountdown(const GamePtr& game,
                             Wt::WContainerWidget* parent) :
    Wt::WViewWidget(parent), game_(game)
{ }

Wt::WWidget* GameCountdown::renderView() {
    try {
        return new GameCountdownImpl(game_);
    } catch (std::exception& e) {
        tApp->log("warning") << e.what();
    }
    return new Wt::WText();
}

}

