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
#include <Wt/Wc/util.hpp>

#include "widgets/game/MyGamesList.hpp"
#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

const int ORDER_OF_STATES_SIZE = 4;
const Game::State ORDER_OF_STATES[ORDER_OF_STATES_SIZE] =
{Game::ACTIVE, Game::PAUSE, Game::CONFIRMED, Game::PROPOSED};

class MyGameAnchor : public Wt::WAnchor, public Notifiable {
public:
    MyGameAnchor(const GamePtr& game, MyGamesListImp* list):
        Notifiable(Object(GAME, game.id())),
        game_(game), list_(list) {
        if (!User::has_s(SWITCH_NAMES_IN_MYMENU) ||
                !game->other_user(tApp->user())) {
            setText(boost::lexical_cast<std::string>(game_.id()));
        } else {
            setText(game->other_user(tApp->user())->safe_username());
        }
        setLink(tApp->path().game_view()->get_link(game_.id()));
        setInline(false);
        excite_or_unexcite();
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
        if (state == Game::ACTIVE) {
            addStyleClass("thechess-active");
        }
        if (state == Game::PAUSE) {
            addStyleClass("thechess-pause");
        }
        if (state == Game::PROPOSED) {
            addStyleClass("thechess-proposed");
        }
        if (state == Game::CONFIRMED) {
            addStyleClass("thechess-confirmed");
        }
    }

private:
    GamePtr game_;
    MyGamesListImp* list_;
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

    void select_game(const GamePtr& game) {
        if (last_clicked_) {
            Anchors::iterator last_it = anchors_.find(last_clicked_);
            if (last_it != anchors_.end()) {
                MyGameAnchor* a = last_it->second;
                a->deselect();
                a->excite_or_unexcite();
            }
        }
        Anchors::iterator it = anchors_.find(game.id());
        if (it != anchors_.end()) {
            MyGameAnchor* target = it->second;
            last_clicked_ = target->game_id();
            target->select();
            target->excite_or_unexcite();
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
    }

    Game::State state_of(MyGameAnchor* a) const {
        int index = indexOf(a);
        for (int i = ORDER_OF_STATES_SIZE - 1; i >= 0; i--)
            if (index >= first_of_state_[i]) {
                return ORDER_OF_STATES[i];
            }
        return Game::MIN_ENDED;
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
        const Object* o = DOWNCAST<const Object*>(e.get());
        if (o->user_id != user_.id()) {
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
        } else {
            a->excite_or_unexcite();
        }
        dbo::Transaction t(tApp->session());
        GamePtr game = a->game();
        game.reread();
        if (game->state() > Game::MIN_ENDED) {
            Wt::Wc::schedule_action(MINUTE, Wt::Wc::bound_post(boost::bind(
                                        &MyGamesListImp::remove_anchor,
                                        this, a, game.id())));
        } else if (state_of(a) != game->state()) {
            extract_anchor(a);
            insert_anchor(a, game->state());
            a->style_by_state(game->state());
        }
    }

    void remove_anchor(MyGameAnchor* a, int game_id) {
        if (indexOf(a) >= 0) {
            extract_anchor(a);
            anchors_.erase(game_id);
            delete a;
            Wt::Wc::updates_trigger();
        }
    }

    friend class MyGameAnchor;
};

void MyGameAnchor::notify(EventPtr e) {
    list_->anchor_notify_handler(this, e);
}

MyGamesList::MyGamesList(const UserPtr& user, Wt::WContainerWidget* p):
    Wt::WCompositeWidget(p) {
    impl_ = new MyGamesListImp(user);
    setImplementation(impl_);
}

void MyGamesList::select_game(const GamePtr& game) {
    impl_->select_game(game);
}

}

