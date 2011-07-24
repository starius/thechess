/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WTable>
#include <Wt/WAnchor>
#include <Wt/WContainerWidget>
#include <Wt/Dbo/Transaction>
namespace dbo = Wt::Dbo;

#include "widgets/MyGamesList.hpp"
#include "model/Game.hpp"
#include "model/Object.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
namespace widgets {
using namespace model;

const int order_of_states_size = 4;
const Game::State order_of_states[order_of_states_size] =
    {Game::active, Game::pause, Game::confirmed, Game::proposed};

class MyGamesListImp;

class MyGameAnchor : public Wt::WAnchor, public Notifiable
{
public:
    MyGameAnchor(const GamePtr& game, MyGamesListImp* list):
    Notifiable(Object(GameObject, game.id())),
    game_id_(game.id()), list_(list)
    {
        setText(boost::lexical_cast<std::string>(game_id_));
        setRefInternalPath(str(boost::format("/game/%i/") % game_id_));
        setInline(false);
        if (game->can_confirm(tApp->user()))
            excite();
        else if (game->can_competition_confirm(tApp->user()))
            excite();
        else if (game->can_pause_agree(tApp->user()))
            excite();
        else if (game->can_mistake_agree(tApp->user()))
            excite();
        else if (game->can_draw_agree(tApp->user()))
            excite();
        else if (game->can_move(tApp->user()))
            excite();
        deselect();
        style_by_state(game->state());
    }

    virtual void notify();

    int game_id() const
    {
        return game_id_;
    }

    void select()
    {
        addStyleClass("thechess-selected");
        removeStyleClass("thechess-deselected");
    }

    void deselect()
    {
        removeStyleClass("thechess-selected");
        addStyleClass("thechess-deselected");
    }

    void excite()
    {
        addStyleClass("thechess-excited");
        removeStyleClass("thechess-unexcited");
    }

    void unexcite()
    {
        removeStyleClass("thechess-excited");
        addStyleClass("thechess-unexcited");
    }

    void style_by_state(Game::State state)
    {
        removeStyleClass("thechess-active");
        removeStyleClass("thechess-pause");
        removeStyleClass("thechess-proposed");
        removeStyleClass("thechess-confirmed");
        if (state == Game::active)
            addStyleClass("thechess-active");
        if (state == Game::pause)
            addStyleClass("thechess-pause");
        if (state == Game::proposed)
            addStyleClass("thechess-proposed");
        if (state == Game::confirmed)
            addStyleClass("thechess-confirmed");
    }

private:
    int game_id_;
    MyGamesListImp* list_;
};

typedef std::map<int, MyGameAnchor*> Anchors;

class MyGamesListImp : public Wt::WContainerWidget, public Notifiable
{
public:
    MyGamesListImp(const UserPtr& user):
    Notifiable(Object(UserObject, user.id())),
    user_id_(user.id()),
    last_clicked_(0)
    {
        for (int i = 0; i < order_of_states_size; i++)
            first_of_state_[i] = 0;
        update_games_list_();
    }

    virtual void notify()
    {
        update_games_list_();
    }

private:
    int user_id_;
    Anchors anchors_;
    int last_clicked_;
    int first_of_state_[order_of_states_size];

    void update_games_list_()
    {
        dbo::Transaction t(tApp->session());
        std::set<int> games;
        UserPtr user = tApp->session().load<User>(user_id_);
        Games games_collection = user->games().where("state < ?").bind(Game::min_ended);
        GamesVector games_vector(games_collection.begin(), games_collection.end());
        BOOST_FOREACH(GamePtr game, games_vector)
        {
            games.insert(game.id());
            if (anchors_.find(game.id()) == anchors_.end())
            {
                MyGameAnchor* a = new MyGameAnchor(game, this);
                a->clicked().connect(this, &MyGamesListImp::click_handler_);
                insert_anchor_(a, game->state());
                anchors_[game.id()] = a;
            }
        }
        t.commit();
    }

    Game::State state_of_(MyGameAnchor* a) const
    {
        int index = indexOf(a);
        for (int i = order_of_states_size-1; i >= 0; i--)
            if (index >= first_of_state_[i])
                return order_of_states[i];
        return Game::min_ended;
    }

    void insert_anchor_(MyGameAnchor* a, Game::State state)
    {
        bool inserted = false;
        for (int i = 0; i < order_of_states_size; i++)
        {
            if (order_of_states[i] == state)
            {
                insertWidget(first_of_state_[i], a);
                inserted = true;
            }
            else if (inserted)
                first_of_state_[i] += 1;
        }
    }

    void extract_anchor_(MyGameAnchor* a)
    {
        int index = indexOf(a);
        removeWidget(a);
        for (int i = 0; i < order_of_states_size; i++)
            if (first_of_state_[i] > index)
                first_of_state_[i] -= 1;
    }

    void anchor_notify_handler_(MyGameAnchor* a)
    {
        a->excite();
        dbo::Transaction t(tApp->session());
        int game_id = a->game_id();
        GamePtr game = tApp->session().load<Game>(game_id);
        game.reread();
        if (game->state() > Game::min_ended)
        {
            extract_anchor_(a);
            anchors_.erase(game_id);
            delete a;
        }
        else if (state_of_(a) != game->state())
        {
            extract_anchor_(a);
            insert_anchor_(a, game->state());
            a->style_by_state(game->state());
        }
        t.commit();
    }

    void click_handler_()
    {
        MyGameAnchor* target = dynamic_cast<MyGameAnchor*>(sender());
        if (last_clicked_)
        {
            Anchors::iterator it = anchors_.find(last_clicked_);
            if (it != anchors_.end())
            {
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

void MyGameAnchor::notify()
{
    list_->anchor_notify_handler_(this);
}

MyGamesList::MyGamesList(UserPtr user, Wt::WContainerWidget* p):
Wt::WCompositeWidget(p)
{
    setImplementation(new MyGamesListImp(user));
}

}
}
