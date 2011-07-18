/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <boost/format.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WTableView>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>
#include <Wt/Dbo/ptr_tuple>
#include <Wt/Dbo/ptr>
namespace dbo = Wt::Dbo;
#include <Wt/WCheckBox>
#include <Wt/WPushButton>
#include <Wt/WString>
#include <Wt/WEnvironment>

#include "widgets/GameListWidget.hpp"
#include "ThechessApplication.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"

namespace thechess {
namespace widgets {
using namespace model;

typedef GamePtr Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int n_column = 0;
const int white_column = 1;
const int black_column = 2;
const int state_column = 3;
const int winner_column = 4;
const int started_column = 5;
const int ended_column = 6;
const int moves_size_column = 7;
const int real_rating_column = 8;
const int comment_column = 9;

class QM : public BaseQM
{
public:
    QM(const Q& query, Wt::WObject *parent=0) :
    BaseQM(parent)
    {
        setQuery(query);
        addColumn("G.id", tr("tc.common.number"));
        addColumn("G.white_id", tr("tc.game.white"));
        addColumn("G.black_id", tr("tc.game.black"));
        addColumn("G.state", tr("tc.game.State"));
        addColumn("G.winner_game_id", tr("tc.common.winner"));
        addColumn("G.started", tr("tc.game.started"));
        addColumn("G.ended", tr("tc.game.ended"));
        addColumn("G.id", tr("tc.game.moves_size")); // dummy G.id
        addColumn("G.norating", tr("tc.game.real_rating"));
        addColumn("G.comment", tr("tc.game.comment"));
        setColumnFlags(n_column, Wt::ItemIsXHTMLText);
    }

    boost::any data(const Wt::WModelIndex& index,
        int role=Wt::DisplayRole) const
    {
        if (role == Wt::DisplayRole)
        {
            dbo::Transaction t(tApp->session());
            GamePtr game = resultRow(index.row());
            if (index.column() == n_column)
            {
                return game.id();
            }
            else if (index.column() == white_column)
            {
                UserPtr user = game->white();
                return user ? user->username() : "";
            }
            else if (index.column() == black_column)
            {
                UserPtr user = game->black();
                return user ? user->username() : "";
            }
            else if (index.column() == state_column)
            {
                return game->str_state();
            }
            else if (index.column() == winner_column)
            {
                UserPtr user = game->winner();
                return user ? user->username() : "";
            }
            else if (index.column() == started_column)
            {
                return game->started();
            }
            else if (index.column() == ended_column)
            {
                return game->ended();
            }
            else if (index.column() == moves_size_column)
            {
                return game->human_size();
            }
            else if (index.column() == real_rating_column)
            {
                return game->real_rating();
            }
            else if (index.column() == comment_column)
            {
                return game->comment();
            }
            t.commit();
        }

        if (role == Wt::InternalPathRole)
        {
            dbo::Transaction t(tApp->session());
            GamePtr game = resultRow(index.row());
            if (index.column() == n_column)
            {
                return str(boost::format("/game/%i/") % game.id());
            }
            else if (index.column() == white_column)
            {
                UserPtr user = game->white();
                return user ? str(boost::format("/user/%i/") % user.id()) : "";
            }
            else if (index.column() == black_column)
            {
                UserPtr user = game->black();
                return user ? str(boost::format("/user/%i/") % user.id()) : "";
            }
            else if (index.column() == winner_column)
            {
                UserPtr user = game->winner();
                return user ? str(boost::format("/user/%i/") % user.id()) : "";
            }
            t.commit();
        }
        return "";
    }

    static Wt::WString tr(const char* key)
    {
        return Wt::WString::tr(key);
    }
};

class GameListWidgetImpl : public Wt::WContainerWidget
{
public:
    GameListWidgetImpl() :
    Wt::WContainerWidget()
    {
        manager_();
        query_model_ = new QM(query(), this);

        table_view_ = new Wt::WTableView(this);
        table_view_->setModel(query_model_);
        table_view_->resize(1200, 300);
        table_view_->setColumnWidth(n_column, 40);
        table_view_->setColumnWidth(white_column, 90);
        table_view_->setColumnWidth(black_column, 90);
        table_view_->setColumnWidth(state_column, 140);
        table_view_->setColumnWidth(winner_column, 90);
        table_view_->setColumnWidth(started_column, 120);
        table_view_->setColumnWidth(ended_column, 120);
        table_view_->setColumnWidth(moves_size_column, 40);
        table_view_->setColumnWidth(real_rating_column, 40);
        table_view_->setColumnWidth(comment_column, 200);
    }

    static Q all_games()
    {
        return tApp->session()
            .query<Result>("select G from thechess_game G");
    }

    Q query()
    {
        dbo::Transaction t(tApp->session());
        Q q = all_games();
        if (only_my_->isChecked() && tApp->user())
        {
            int id = tApp->user()->id();
            q.where("G.white_id = ? or G.black_id = ? or G.init_game_id = ?")
            .bind(id).bind(id).bind(id);
        }
        q.orderBy("G.id");
        t.commit();
        return q;
    }

private:
    QM* query_model_;
    Wt::WTableView* table_view_;
    Wt::WCheckBox* only_my_;

    void manager_()
    {
        only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
        only_my_->changed().connect(this, &GameListWidgetImpl::apply_);
        if (!tApp->user())
        {
            only_my_->setEnabled(false);
        }
        if (!tApp->environment().ajax())
        {
            Wt::WPushButton* apply_button =
                new Wt::WPushButton(tr("tc.common.Apply"), this);
            apply_button->clicked().connect(this, &GameListWidgetImpl::apply_);
        }
    }

    void apply_()
    {
        query_model_->setQuery(query(), /* keep_columns */ true);
    }

};

GameListWidget::GameListWidget(Wt::WContainerWidget* parent) :
WCompositeWidget(parent)
{
    impl_ = new GameListWidgetImpl();
    setImplementation(impl_);
}

}
}
