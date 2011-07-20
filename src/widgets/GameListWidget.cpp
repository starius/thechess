/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>

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

typedef boost::tuple<GamePtr, Wt::WString, Wt::WString, Wt::WString> Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int n_column = 0;
const int white_column = 1;
const int black_column = 2;
const int state_column = 3;
const int winner_column = 4;
const int started_column = 5;
const int real_rating_column = 6;
const int moves_size_column = 7;
const int comment_column = 8;

const int game_in_tuple = 0;

class QM : public BaseQM
{
public:
    QM(const Q& query, Wt::WObject *parent=0) :
    BaseQM(parent)
    {
        setQuery(query);
        addColumn("G.id", tr("tc.common.number"));
        addColumn("Wh.username", tr("tc.game.white"));
        addColumn("B.username", tr("tc.game.black"));
        addColumn("G.state", tr("tc.game.State")); // dummy
        addColumn("Wi.username", tr("tc.common.winner"));
        addColumn("G.started", tr("tc.game.started"));
        addColumn("G.norating", tr("tc.game.real_rating")); // dummy
        addColumn("G.id", tr("tc.game.moves_size")); // dummy
        addColumn("G.comment", tr("tc.game.comment"));
    }

    boost::any data(const Wt::WModelIndex& index,
        int role=Wt::DisplayRole) const
    {
        dbo::Transaction t(tApp->session());
        GamePtr game = resultRow(index.row()).get<game_in_tuple>();
        if (role == Wt::DisplayRole)
        {
            if (index.column() == state_column)
                return game->str_state();
            else if (index.column() == moves_size_column)
                return game->human_size();
            else if (index.column() == real_rating_column)
                return game->real_rating();
        }
        else if (role == Wt::InternalPathRole)
        {
            UserPtr user;
            if (index.column() == n_column)
                return str(boost::format("/game/%i/") % game.id());
            else if (index.column() == white_column)
                user = game->white();
            else if (index.column() == black_column)
                user = game->black();
            else if (index.column() == winner_column)
                user = game->winner();
            if (user.id() > 0) // FIXME http://redmine.webtoolkit.eu/issues/909
                return str(boost::format("/user/%i/") % user.id());
        }
        t.commit();
        return BaseQM::data(index, role);
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
        table_view_->resize(770, 450);
        table_view_->setColumnWidth(n_column, 65);
        table_view_->setColumnWidth(white_column, 75);
        table_view_->setColumnWidth(black_column, 75);
        table_view_->setColumnWidth(state_column, 80);
        table_view_->setColumnWidth(winner_column, 120);
        table_view_->setColumnWidth(started_column, 70);
        table_view_->setColumnWidth(real_rating_column, 40);
        table_view_->setColumnWidth(moves_size_column, 40);
        table_view_->setColumnWidth(comment_column, 120);
    }

    static Q all_games()
    {
        return tApp->session()
            .query<Result>("select G, Wh.username, B.username, Wi.username "
            "from thechess_game G "
            "left join thechess_user Wh on G.white_id=Wh.id "
            "left join thechess_user B on G.black_id=B.id "
            "left join thechess_user Wi on G.winner_game_id=Wi.id ");
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
