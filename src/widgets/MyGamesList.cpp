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
                anchors_[game.id()] = a;
                addWidget(a);
            }
        }
        t.commit();
    }

    void remove_anchor_(MyGameAnchor* a)
    {
        anchors_.erase(a->game_id());
        removeWidget(a);
        delete a;
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
    dbo::Transaction t(tApp->session());
    GamePtr game = tApp->session().load<Game>(game_id_);
    game.reread();
    if (game->state() > Game::min_ended)
        list_->remove_anchor_(this);
    else
        excite();
    t.commit();
}

MyGamesList::MyGamesList(UserPtr user, Wt::WContainerWidget* p):
Wt::WCompositeWidget(p)
{
    setImplementation(new MyGamesListImp(user));
}

}
}

