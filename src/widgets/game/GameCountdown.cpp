/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTemplate>
#include <Wt/WContainerWidget>
#include <Wt/WAnchor>
#include <Wt/Wc/Countdown.hpp>
#include <Wt/Wc/util.hpp>

#include "chess/Piece.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "widgets/game/GameCountdown.hpp"

namespace thechess {

class GameCountdown::SingleTimeout : public Wt::WTemplate {
public:
    SingleTimeout(const UserPtr& user, GameCountdown* parent = 0):
        Wt::WTemplate(tr("tc.game.countdown_template"), parent),
        user_(user) {
        dbo::Transaction t(tApp->session());
        bindWidget("name", anchor());
        bindWidget("limit_private", new Wt::Wc::Countdown());
        bindWidget("limit_std", new Wt::Wc::Countdown());
        reread();
        t.commit();
    }

    void update_user(const UserPtr& user) {
        if (user != user_) {
            user_ = user;
            bindWidget("name", anchor());
        }
        reread();
    }

    Wt::Wc::Countdown* limit_private() {
        return downcast<Wt::Wc::Countdown*>(resolveWidget("limit_private"));
    }

    Wt::Wc::Countdown* limit_std() {
        return downcast<Wt::Wc::Countdown*>(resolveWidget("limit_std"));
    }

private:
    UserPtr user_;

    GameCountdown* game_countdown() {
        return downcast<GameCountdown*>(parent());
    }

    const GamePtr& game() {
        return game_countdown()->game();
    }

    void reread() {
        dbo::Transaction t(tApp->session());
        limit_private()->pause();
        limit_private()->set_until(game()->limit_private_now(user_));
        limit_std()->pause();
        limit_std()->set_until(game()->limit_std_now(user_));
        if (game()->state() == Game::ACTIVE && user_ == game()->order_user()) {
            limit_std()->resume();
            limit_private()->resume(game()->limit_std_now(user_));
        }
        t.commit();
    }

    Wt::WAnchor* anchor() {
        Wt::WAnchor* result = new Wt::WAnchor();
        result->setText(user_->safe_username());
        result->setLink(tApp->path().user_view()->get_link(user_.id()));
        return result;
    }
};

GameCountdown::GameCountdown(const GamePtr& game,
                             Wt::WContainerWidget* parent) :
    Wt::WContainerWidget(parent),
    game_(game),
    timeout1_(0), timeout2_(0) {
    reread();
}

void GameCountdown::reread() {
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
    single_countdown(timeout1_, user1);
    single_countdown(timeout2_, user2);
    t.commit();
}

void GameCountdown::single_countdown(SingleTimeout*& timeout,
                                     const UserPtr& user) {
    if (user) {
        if (timeout) {
            timeout->update_user(user);
        } else {
            timeout = new SingleTimeout(user, this);
        }
    }
}

}

