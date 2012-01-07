/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WTable>
#include <Wt/WAnchor>
#include <Wt/WContainerWidget>
#include <Wt/Dbo/Transaction>
#include <Wt/Wc/util.hpp>

#include "widgets/MyGamesList.hpp"
#include "model/all.hpp"
#include "Application.hpp"
#include "notify.hpp"

namespace thechess {

const int ORDER_OF_STATES_SIZE = 4;
const Game::State ORDER_OF_STATES[ORDER_OF_STATES_SIZE] =
{Game::ACTIVE, Game::PAUSE, Game::CONFIRMED, Game::PROPOSED};

class MyGamesListImp;

class MyGameAnchor : public Wt::WAnchor, public Notifiable {
public:
    MyGameAnchor(const GamePtr& game, MyGamesListImp* list):
        Notifiable(Object(GAME, game.id()), tNot),
        game_id_(game.id()), list_(list) {
        setText(boost::lexical_cast<std::string>(game_id_));
        setRefInternalPath(str(boost::format("/game/%i/") % game_id_));
        setInline(false);
        UserPtr user = tApp->user();
        if (game->can_confirm(user)) {
            excite();
        } else if (game->can_competition_confirm(user)) {
            excite();
        } else if (game->can_pause_agree(user)) {
            excite();
        } else if (game->can_mistake_agree(user)) {
            excite();
        } else if (game->can_draw_agree(user)) {
            excite();
        } else if (game->can_move(user)) {
            excite();
        }
        deselect();
        style_by_state(game->state());
    }

    virtual void notify(EventPtr);

    int game_id() const {
        return game_id_;
    }

    void select() {
        addStyleClass("thechess-selected");
        removeStyleClass("thechess-deselected");
    }

    void deselect() {
        removeStyleClass("thechess-selected");
        addStyleClass("thechess-deselected");
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
    int game_id_;
    MyGamesListImp* list_;
};

typedef std::map<int, MyGameAnchor*> Anchors;

class MyGamesListImp : public Wt::WContainerWidget, public Notifiable {
public:
    MyGamesListImp(const UserPtr& user):
        Notifiable(Object(USER, user.id()), tNot),
        user_(user),
        last_clicked_(0) {
        for (int i = 0; i < ORDER_OF_STATES_SIZE; i++) {
            first_of_state_[i] = 0;
        }
        update_games_list();
    }

    void notify(EventPtr) {
        update_games_list();
    }

private:
    UserPtr user_;
    Anchors anchors_;
    int last_clicked_;
    int first_of_state_[ORDER_OF_STATES_SIZE];

    void update_games_list() {
        dbo::Transaction t(tApp->session());
        user_.reread();
        Games games = user_->games().where("state < ?").bind(Game::MIN_ENDED);
        BOOST_FOREACH (GamePtr game, games) {
            if (anchors_.find(game.id()) == anchors_.end()) {
                MyGameAnchor* a = new MyGameAnchor(game, this);
                a->clicked().connect(this, &MyGamesListImp::click_handler);
                insert_anchor_(a, game->state());
                anchors_[game.id()] = a;
            }
        }
        t.commit();
    }

    Game::State state_of_(MyGameAnchor* a) const {
        int index = indexOf(a);
        for (int i = ORDER_OF_STATES_SIZE - 1; i >= 0; i--)
            if (index >= first_of_state_[i]) {
                return ORDER_OF_STATES[i];
            }
        return Game::MIN_ENDED;
    }

    void insert_anchor_(MyGameAnchor* a, Game::State state) {
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

    void extract_anchor_(MyGameAnchor* a) {
        int index = indexOf(a);
        removeWidget(a);
        for (int i = 0; i < ORDER_OF_STATES_SIZE; i++)
            if (first_of_state_[i] > index) {
                first_of_state_[i] -= 1;
            }
    }

    void anchor_notify_handler_(MyGameAnchor* a) {
        a->excite();
        dbo::Transaction t(tApp->session());
        int game_id = a->game_id();
        GamePtr game = tApp->session().load<Game>(game_id);
        game.reread();
        if (game->state() > Game::MIN_ENDED) {
            extract_anchor_(a);
            anchors_.erase(game_id);
            delete a;
        } else if (state_of_(a) != game->state()) {
            extract_anchor_(a);
            insert_anchor_(a, game->state());
            a->style_by_state(game->state());
        }
        t.commit();
    }

    void click_handler() {
        MyGameAnchor* target = downcast<MyGameAnchor*>(sender());
        if (last_clicked_) {
            Anchors::iterator it = anchors_.find(last_clicked_);
            if (it != anchors_.end()) {
                MyGameAnchor* a = it->second;
                a->deselect();
                a->unexcite();
            }
        }
        last_clicked_ = target->game_id();
        target->select();
        target->unexcite();
    }

    friend class MyGameAnchor;
};

void MyGameAnchor::notify(EventPtr) {
    list_->anchor_notify_handler_(this);
}

MyGamesList::MyGamesList(const UserPtr& user, Wt::WContainerWidget* p):
    Wt::WCompositeWidget(p) {
    setImplementation(new MyGamesListImp(user));
}

}

