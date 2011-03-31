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

#include "widgets/GameListWidget.hpp"
#include "ThechessApplication.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"

namespace thechess {
namespace widgets {

using model::Game;
using model::User;
typedef dbo::ptr_tuple<Game, User, User, User>::type Result;
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

const int game_in_tuple = 0;
const int white_in_tuple = 1;
const int black_in_tuple = 2;
const int winner_in_tuple = 3;

class QM : public BaseQM
{
public:
    QM(Wt::WObject *parent=0) :
    BaseQM(parent)
    {
    }

    boost::any data(const Wt::WModelIndex& index,
        int role=Wt::DisplayRole) const
    {
        if (index.column() == state_column && role == Wt::DisplayRole)
        {
            GamePtr game = resultRow(index.row()).get<0>();
            return game->str_state();
        }
        else if (index.column() == moves_size_column && role == Wt::DisplayRole)
        {
            GamePtr game = resultRow(index.row()).get<0>();
            return game->human_size();
        }
        else if (index.column() == real_rating_column && role == Wt::DisplayRole)
        {
            GamePtr game = resultRow(index.row()).get<0>();
            return game->real_rating();
        }
        else
        {
            return BaseQM::data(index, role);
        }
    }
};

class GameListWidgetImpl : public Wt::WContainerWidget
{
public:
    GameListWidgetImpl() :
    Wt::WContainerWidget()
    {
        manager_();
        query_model_ = new QM(this);
        query_model_->setQuery(query());

        query_model_->addColumn("G.id", tr("thechess.number"));
        query_model_->addColumn("Uw.username", tr("thechess.white"));
        query_model_->addColumn("Ub.username", tr("thechess.black"));
        query_model_->addColumn("G.state", tr("thechess.state"));
        query_model_->addColumn("Uwin.username", tr("thechess.winner"));
        query_model_->addColumn("G.started", tr("thechess.started"));
        query_model_->addColumn("G.ended", tr("thechess.ended"));
        query_model_->addColumn("G.moves_size", tr("thechess.moves_size"));
        query_model_->addColumn("G.norating", tr("thechess.real_rating"));

        table_view_ = new Wt::WTableView(this);
        table_view_->setModel(query_model_);
        table_view_->resize(900, 300);

        table_view_->setColumnWidth(n_column, 40);
        table_view_->setColumnWidth(white_column, 90);
        table_view_->setColumnWidth(black_column, 90);
        table_view_->setColumnWidth(state_column, 140);
        table_view_->setColumnWidth(winner_column, 90);
        table_view_->setColumnWidth(started_column, 120);
        table_view_->setColumnWidth(ended_column, 120);
        table_view_->setColumnWidth(moves_size_column, 40);
        table_view_->setColumnWidth(real_rating_column, 40);

        table_view_->clicked()
            .connect(this, &GameListWidgetImpl::clicked_handler_);
    }

    static Q all_games()
    {
        return tApp->session().query<Result>
        ("select G, Uw, Ub, Uwin from thechess_game G "
         "left outer join thechess_user Uw on G.white_id = Uw.id "
         "left outer join thechess_user Ub on G.black_id = Ub.id "
         "left outer join thechess_user Uwin on G.winner_game_id = Uwin.id");
    }

    Q query()
    {
        dbo::Transaction t(tApp->session());
        Q q = all_games();
        if (only_my_->isChecked() && tApp->user())
        {
            int id = tApp->user()->id();
            q.where("G.white_id = ? or G.black_id = ? or G.init_id = ?")
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
        only_my_ = new Wt::WCheckBox(tr("thechess.only_my"), this);
        only_my_->changed().connect(this, &GameListWidgetImpl::apply_);
        if (!tApp->user())
        {
            only_my_->setEnabled(false);
        }

        Wt::WPushButton* apply_button =
            new Wt::WPushButton(tr("thechess.apply"), this);
        apply_button->clicked().connect(this, &GameListWidgetImpl::apply_);
    }

    void apply_()
    {
        query_model_->setQuery(query(), /* keep_columns */ true);
    }

    void clicked_handler_(const Wt::WModelIndex index)
    {
        const Result& r = query_model_->resultRow(index.row());
        GamePtr game = r.get<game_in_tuple>();
        if (index.column() == n_column)
        {
            tApp->view(game);
        }
        else if (index.column() == white_column ||
            index.column() == black_column ||
            index.column() == winner_column)
        {
            UserPtr user;
            if (index.column() == white_column)
            {
                user = game->white();
            }
            else if (index.column() == black_column)
            {
                user = game->black();
            }
            else if (index.column() == winner_column)
            {
                user = game->winner();
            }
            if (user)
            {
                tApp->view(user);
            }
        }
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
