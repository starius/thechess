/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WTable>
#include <Wt/WSound>
#include <Wt/WAnchor>
#include <Wt/WContainerWidget>
#include <Wt/Dbo/Transaction>
#include <Wt/Wc/Countdown.hpp>
#include <Wt/Wc/util.hpp>

#include "widgets/game/MyGamesList.hpp"
#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

const int ORDER_OF_STATES_SIZE = 4;
const Game::State ORDER_OF_STATES[ORDER_OF_STATES_SIZE] =
{Game::ACTIVE, Game::PAUSE, Game::CONFIRMED, Game::PROPOSED};

class MyGameAnchor : public Wt::WContainerWidget, public Notifiable {
public:
    MyGameAnchor(const GamePtr& game, MyGamesListImp* list):
        Notifiable(Object(GAME, game.id())),
        game_(game), list_(list) {
        anchor_ = new Wt::WAnchor(this);
        if (!User::has_s(SWITCH_NAMES_IN_MYMENU) ||
                !game->other_user(tApp->user())) {
            anchor_->setText(boost::lexical_cast<std::string>(game_.id()));
        } else {
            anchor_->setText(game->other_user(tApp->user())->safe_username());
        }
        anchor_->setLink(tApp->path().game_view()->get_link(game_.id()));
        anchor_->setMargin(5, Wt::Left | Wt::Right);
        online_ = new Wt::WText(tr("tc.user.Online"), this);
        online_->addStyleClass("no-wrap");
        countdown_ = new Wt::Wc::Countdown(this, /* load JS */ false);
        countdown_->addStyleClass("no-wrap");
        countdown_->setMargin(5, Wt::Left | Wt::Right);
        if (game_->competition()) {
            new Wt::WText(tr("tc.competition.c"), this);
        }
        excite_or_unexcite();
        update_countdown();
        deselect();
        style_by_state(game->state());
    }

    virtual void notify(EventPtr);

    bool can_i_do_smth() {
        dbo::Transaction t(tApp->session());
        UserPtr user = tApp->user();
        return game_->can_confirm(user) ||
               game_->can_competition_confirm(user) ||
               game_->can_pause_agree(user) ||
               game_->can_mistake_agree(user) ||
               game_->can_draw_agree(user) ||
               game_->can_move(user);
    }

    const GamePtr& game() const {
        return game_;
    }

    int game_id() const {
        return game_.id();
    }

    void select() {
        addStyleClass("thechess-selected");
        removeStyleClass("thechess-deselected");
    }

    void deselect() {
        removeStyleClass("thechess-selected");
        addStyleClass("thechess-deselected");
    }

    void excite_or_unexcite() {
        if (can_i_do_smth()) {
            excite();
        } else {
            unexcite();
        }
        update_online();
    }

    void update_online() {
        UserPtr competitor = game_->other_user(tApp->user());
        online_->setHidden(!competitor || !competitor->online());
    }

    void update_countdown() {
        dbo::Transaction t(tApp->session());
        countdown_->pause();
        Td duration = game_->total_limit_now(tApp->user());
        if (duration > TD_NULL) {
            countdown_->set_until(duration);
        }
        if (game_->state() == Game::ACTIVE &&
                tApp->user() == game_->order_user()) {
            countdown_->resume();
        }
    }

    void excite() {
        addStyleClass("thechess-excited");
        removeStyleClass("thechess-unexcited");
    }

    void unexcite() {
        removeStyleClass("thechess-excited");
        addStyleClass("thechess-unexcited");
    }

    void style_by_state(Game::State state) {
        removeStyleClass("thechess-active");
        removeStyleClass("thechess-pause");
        removeStyleClass("thechess-proposed");
        removeStyleClass("thechess-confirmed");
        show();
        if (state == Game::ACTIVE) {
            addStyleClass("thechess-active");
        }
        if (state == Game::PAUSE) {
            addStyleClass("thechess-pause");
            if (User::has_s(SWITCH_HIDE_PAUSED_GAMES)) {
                hide();
            }
        }
        if (state == Game::PROPOSED) {
            addStyleClass("thechess-proposed");
        }
        if (state == Game::CONFIRMED) {
            addStyleClass("thechess-confirmed");
        }
    }

    void check_state();

    void check_ended();

private:
    GamePtr game_;
    MyGamesListImp* list_;
    Wt::WAnchor* anchor_;
    Wt::WText* online_;
    Wt::Wc::Countdown* countdown_;
};

typedef std::map<int, MyGameAnchor*> Anchors;

class MyGamesListImp : public Wt::WContainerWidget, public Notifiable {
public:
    MyGamesListImp(const UserPtr& user):
        Notifiable(Object(USER, user.id())),
        user_(user),
        last_clicked_(0) {
        sound_ = new Wt::WSound("/sound/glass.mp3", this);
        for (int i = 0; i < ORDER_OF_STATES_SIZE; i++) {
            first_of_state_[i] = 0;
        }
        update_games_list();
    }

    void notify(EventPtr) {
        update_games_list();
    }

    void select_game(int game_id) {
        if (last_clicked_) {
            Anchors::iterator last_it = anchors_.find(last_clicked_);
            if (last_it != anchors_.end()) {
                MyGameAnchor* a = last_it->second;
                a->deselect();
                a->excite_or_unexcite();
                a->check_state();
                a->check_ended();
            }
        }
        Anchors::iterator it = anchors_.find(game_id);
        if (it != anchors_.end()) {
            MyGameAnchor* target = it->second;
            last_clicked_ = target->game_id();
            target->select();
            target->excite_or_unexcite();
            target->check_state();
        }
    }

private:
    UserPtr user_;
    Anchors anchors_;
    int last_clicked_;
    int first_of_state_[ORDER_OF_STATES_SIZE];
    Wt::WSound* sound_;

    void update_games_list() {
        dbo::Transaction t(tApp->session());
        user_.reread();
        Games games = user_->games().where("state < ?").bind(Game::MIN_ENDED);
        BOOST_FOREACH (GamePtr game, games) {
            if (anchors_.find(game.id()) == anchors_.end()) {
                MyGameAnchor* a = new MyGameAnchor(game, this);
                insert_anchor(a, game->state());
                anchors_[game.id()] = a;
            }
        }
        try {
            int game_id = tApp->path().game_view()->integer();
            select_game(game_id);
        } catch (...)
        { }
    }

    Game::State state_of(MyGameAnchor* a) const {
        int index = indexOf(a);
        for (int i = ORDER_OF_STATES_SIZE - 1; i >= 0; i--)
            if (index >= first_of_state_[i]) {
                return ORDER_OF_STATES[i];
            }
        throw std::logic_error("MyGamesListImp: bad anchor state");
    }

    void insert_anchor(MyGameAnchor* a, Game::State state) {
        bool inserted = false;
        for (int i = 0; i < ORDER_OF_STATES_SIZE; i++) {
            if (ORDER_OF_STATES[i] == state) {
                insertWidget(first_of_state_[i], a);
                inserted = true;
            } else if (inserted) {
                first_of_state_[i] += 1;
            }
        }
    }

    void extract_anchor(MyGameAnchor* a) {
        int index = indexOf(a);
        removeWidget(a);
        for (int i = 0; i < ORDER_OF_STATES_SIZE; i++)
            if (first_of_state_[i] > index) {
                first_of_state_[i] -= 1;
            }
    }

    void anchor_notify_handler(MyGameAnchor* a, EventPtr e) {
        dbo::Transaction t(tApp->session());
        const Object* o = DOWNCAST<const Object*>(e.get());
        if (o->user_id != user_.id()) {
            GamePtr game = a->game();
            game.reread();
            MyGameAnchor* last_clicked_a = 0;
            if (last_clicked_) {
                Anchors::iterator it = anchors_.find(last_clicked_);
                if (it != anchors_.end()) {
                    last_clicked_a = it->second;
                }
            }
            if (a != last_clicked_a) {
                a->excite();
            }
            sound_->stop();
            sound_->play();
            a->check_state();
        } else {
            a->excite_or_unexcite();
        }
        a->update_countdown();
        a->update_online();
    }

    void remove_anchor(MyGameAnchor* a, int game_id) {
        if (indexOf(a) >= 0) {
            extract_anchor(a);
            anchors_.erase(game_id);
            delete a;
        }
    }

    friend class MyGameAnchor;
};

void MyGameAnchor::notify(EventPtr e) {
    list_->anchor_notify_handler(this, e);
}

void MyGameAnchor::check_state() {
    dbo::Transaction t(tApp->session());
    if (list_->state_of(this) != game_->state()) {
        if (!game_->is_ended()) {
            list_->extract_anchor(this);
            list_->insert_anchor(this, game_->state());
        }
        style_by_state(game_->state());
    }
}

void MyGameAnchor::check_ended() {
    dbo::Transaction t(tApp->session());
    if (game_->state() > Game::MIN_ENDED) {
        list_->remove_anchor(this, game_.id());
    }
}

MyGamesList::MyGamesList(const UserPtr& user, Wt::WContainerWidget* p):
    Wt::WCompositeWidget(p) {
    impl_ = new MyGamesListImp(user);
    setImplementation(impl_);
}

void MyGamesList::select_game(const GamePtr& game) {
    impl_->select_game(game.id());
}

}

