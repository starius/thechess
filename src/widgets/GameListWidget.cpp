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
#include "model/all.hpp"

namespace thechess {

namespace GLP {
typedef boost::tuple<GamePtr, bool, Wt::WString, Wt::WString, Wt::WString> Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int GAME_IN_TUPLE = 0;
}

class GameListModel : public GLP::BaseQM {
public:
    static const int N_COLUMN = 0;
    static const int WHITE_COLUMN = 1;
    static const int BLACK_COLUMN = 2;
    static const int STATE_COLUMN = 3;
    static const int WINNER_COLUMN = 4;
    static const int STARTED_COLUMN = 5;
    static const int REAL_RATING_COLUMN = 6;
    static const int MOVES_SIZE_COLUMN = 7;
    static const int COMMENT_COLUMN = 8;

    GameListModel(const GLP::Q& query, Wt::WObject* parent = 0) :
        GLP::BaseQM(parent) {
        setQuery(query);
        addColumn("G.id", tr("tc.common.number"));
        addColumn("Wh.username", tr("tc.game.white"));
        addColumn("B.username", tr("tc.game.black"));
        addColumn("G.state", tr("tc.game.State")); // dummy
        addColumn("Wi.username", tr("tc.common.winner"));
        addColumn("G.started", tr("tc.game.started"));
        addColumn("GP.norating", tr("tc.game.real_rating")); // dummy
        addColumn("G.id", tr("tc.game.moves_size")); // dummy
        addColumn("G.comment", tr("tc.game.comment"));
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        GamePtr game = resultRow(index.row()).get<GLP::GAME_IN_TUPLE>();
        if (role == Wt::DisplayRole) {
            if (index.column() == STATE_COLUMN) {
                return game->str_state();
            } else if (index.column() == MOVES_SIZE_COLUMN) {
                return game->moves_number();
            } else if (index.column() == REAL_RATING_COLUMN) {
                return game->real_rating();
            }
        } else if (role == Wt::InternalPathRole) {
            UserPtr user;
            if (index.column() == N_COLUMN) {
                return str(boost::format("/game/%i/") % game.id());
            } else if (index.column() == WHITE_COLUMN) {
                user = game->white();
            } else if (index.column() == BLACK_COLUMN) {
                user = game->black();
            } else if (index.column() == WINNER_COLUMN) {
                user = game->winner();
            }
            if (user.id() > 0) { // FIXME http://redmine.webtoolkit.eu/issues/909
                return str(boost::format("/user/%i/") % user.id());
            }
        }
        t.commit();
        return GLP::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class GameListWidgetImpl : public Wt::WContainerWidget {
public:
    GameListWidgetImpl() :
        Wt::WContainerWidget() {
        manager_();
        query_model_ = new GameListModel(query(), this);
        table_view_ = new Wt::WTableView(this);
        table_view_->setModel(query_model_);
        table_view_->resize(770, 450);
        table_view_->setColumnWidth(GameListModel::N_COLUMN, 65);
        table_view_->setColumnWidth(GameListModel::WHITE_COLUMN, 75);
        table_view_->setColumnWidth(GameListModel::BLACK_COLUMN, 75);
        table_view_->setColumnWidth(GameListModel::STATE_COLUMN, 80);
        table_view_->setColumnWidth(GameListModel::WINNER_COLUMN, 120);
        table_view_->setColumnWidth(GameListModel::STARTED_COLUMN, 70);
        table_view_->setColumnWidth(GameListModel::REAL_RATING_COLUMN, 40);
        table_view_->setColumnWidth(GameListModel::MOVES_SIZE_COLUMN, 40);
        table_view_->setColumnWidth(GameListModel::COMMENT_COLUMN, 120);
    }

    static GLP::Q all_games() {
        return tApp->session()
               .query<GLP::Result>("select G, GP.norating, Wh.username, B.username, Wi.username "
                                   "from thechess_game G "
                                   "left join thechess_gp GP on G.gp_id=GP.id "
                                   "left join thechess_user Wh on G.white_id=Wh.id "
                                   "left join thechess_user B on G.black_id=B.id "
                                   "left join thechess_user Wi on G.winner_game_id=Wi.id ");
    }

    GLP::Q query() {
        dbo::Transaction t(tApp->session());
        GLP::Q q = all_games();
        if (only_my_->isChecked() && tApp->user()) {
            int id = tApp->user()->id();
            q.where("G.white_id = ? or G.black_id = ? or G.init_game_id = ?")
            .bind(id).bind(id).bind(id);
        }
        q.orderBy("G.id");
        t.commit();
        return q;
    }

private:
    GameListModel* query_model_;
    Wt::WTableView* table_view_;
    Wt::WCheckBox* only_my_;

    void manager_() {
        only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
        only_my_->changed().connect(this, &GameListWidgetImpl::apply_);
        if (!tApp->user()) {
            only_my_->setEnabled(false);
        }
        if (!tApp->environment().ajax()) {
            Wt::WPushButton* apply_button =
                new Wt::WPushButton(tr("tc.common.Apply"), this);
            apply_button->clicked().connect(this, &GameListWidgetImpl::apply_);
        }
    }

    void apply_() {
        query_model_->setQuery(query(), /* keep_columns */ true);
    }

};

GameListWidget::GameListWidget(Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new GameListWidgetImpl();
    setImplementation(impl_);
}

}

