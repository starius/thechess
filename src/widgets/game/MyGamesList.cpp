/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WSound>
#include <Wt/WImage>
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

class OnlineLabel : public Wt::WText, public Notifiable {
public:
    OnlineLabel(const UserPtr& user):
        Wt::WText(tr("tc.user.Online")),
        Notifiable(Object(USER, user.id())),
        user_(user) {
        update_hidden();
    }

    void notify(EventPtr) {
        dbo::Transaction t(tApp->session());
        user_.reread();
        update_hidden();
    }

    void update_hidden() {
        setHidden(!user_->online());
    }

private:
    UserPtr user_;
};

class MyGameAnchor : public Wt::WContainerWidget, public Notifiable {
public:
    MyGameAnchor(const GamePtr& game, MyGamesListImp* list):
        Notifiable(Object(GAME, game.id())),
        game_(game), list_(list),
        my_countdown_(0), competitor_countdown_(0) {
        addStyleClass("no-wrap");
        reprint();
    }

    void reprint() {
        clear();
        my_countdown_ = 0;
        competitor_countdown_ = 0;
        Piece::Color my_color = game_->color_of(tApp->user());
        Wt::WImage* color = 0;
        if (my_color == Piece::WHITE) {
            color = new Wt::WImage("img/chess/white.gif", this);
        } else if (my_color == Piece::BLACK) {
            color = new Wt::WImage("img/chess/black.gif", this);
        }
        anchor_ = new Wt::WAnchor(this);
        if (!User::has_s(SWITCH_NAMES_IN_MYMENU) ||
                !game_->other_user(tApp->user())) {
            anchor_->setText(boost::lexical_cast<std::string>(game_.id()));
        } else {
            anchor_->setText(game_->other_user(tApp->user())->safe_username());
        }
        anchor_->setLink(tApp->path().game_view()->get_link(game_.id()));
        anchor_->setMargin(5, Wt::Left | Wt::Right);
        if (!User::has_s(SWITCH_HIDE_ONLINE)) {
            UserPtr competitor = game_->other_user(tApp->user());
            if (competitor) {
                addWidget(new OnlineLabel(competitor));
            }
        }
        if (game_->competition()) {
            Wt::WText* c = new Wt::WText(tr("tc.competition.c"), this);
            c->addStyleClass("thechess-c");
        }
        add_countdown();
        excite_or_unexcite();
        update_countdown();
        deselect();
        style_by_state(game_->state());
    }

    void add_countdown() {
        if (!User::has_s(SWITCH_HIDE_COUNTDOWN)) {
            my_countdown_ = new Wt::Wc::Countdown(this, /* load JS */ false);
            my_countdown_->setMargin(5, Wt::Left | Wt::Right);
        }
        if (User::has_s(SWITCH_COMPETOR_TIME)) {
            competitor_countdown_ = new Wt::Wc::Countdown(this, false);
            competitor_countdown_->setMargin(5, Wt::Left | Wt::Right);
        }
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
    }

    void update_countdown() {
        if (my_countdown_) {
            dbo::Transaction t(tApp->session());
            my_countdown_->pause();
            Td duration = game_->total_limit_now(tApp->user());
            if (duration > TD_NULL) {
                my_countdown_->set_until(duration);
            }
            if (game_->state() == Game::ACTIVE &&
                    tApp->user() == game_->order_user()) {
                my_countdown_->resume();
            }
        }
        if (competitor_countdown_) {
            dbo::Transaction t(tApp->session());
            competitor_countdown_->pause();
            UserPtr competitor = game_->other_user(tApp->user());
            Td duration = game_->total_limit_now(competitor);
            if (duration > TD_NULL) {
                competitor_countdown_->set_until(duration);
            }
            if (game_->state() == Game::ACTIVE &&
                    competitor == game_->order_user()) {
                competitor_countdown_->resume();
            }
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
        if (state >= Game::MIN_ENDED) {
            addStyleClass("thechess-ended");
        }
    }

    void check_state();

    void check_ended();

private:
    GamePtr game_;
    MyGamesListImp* list_;
    Wt::WAnchor* anchor_;
    Wt::Wc::Countdown* my_countdown_;
    Wt::Wc::Countdown* competitor_countdown_;
};

typedef std::map<int, MyGameAnchor*> Anchors;

class MyGamesListImp : public Wt::WContainerWidget, public Notifiable {
public:
    MyGamesListImp(const UserPtr& user):
        Notifiable(Object(USER, user.id())),
        user_(user),
        last_clicked_(0) {
        sound_ = new Wt::WSound("/sound/tick.mp3", this);
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
            a->excite_or_unexcite();
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
        reprint();
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

