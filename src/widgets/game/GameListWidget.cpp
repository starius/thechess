/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
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
#include <Wt/Wc/Pager.hpp>

#include <Wt/WCheckBox>
#include <Wt/WPushButton>
#include <Wt/WString>
#include <Wt/WEnvironment>

#include "widgets/game/GameListWidget.hpp"
#include "Application.hpp"
#include "model/all.hpp"

namespace thechess {

namespace GLP {
typedef Wt::WString S;
typedef boost::tuple<GamePtr, bool, S, S, S> Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int GAME_IN_TUPLE = 0;
}

class GameListModel : public GLP::BaseQM {
public:
    enum {
        N_COLUMN,
        WHITE_COLUMN,
        BLACK_COLUMN,
        STATE_COLUMN,
        WINNER_COLUMN,
        STARTED_COLUMN,
        REAL_RATING_COLUMN,
        MOVES_SIZE_COLUMN,
        COMMENT_COLUMN
    };

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
        addColumn("G.name", tr("tc.game.comment"));
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const GamePtr& game = resultRow(index.row()).get<GLP::GAME_IN_TUPLE>();
        if (role == Wt::DisplayRole) {
            if (index.column() == STATE_COLUMN) {
                return game->str_state();
            } else if (index.column() == MOVES_SIZE_COLUMN) {
                return game->moves_number();
            } else if (index.column() == REAL_RATING_COLUMN) {
                return game->real_rating();
            }
        } else if (role == Wt::LinkRole) {
            UserPtr user;
            if (index.column() == N_COLUMN) {
                return tApp->path().game_view()->get_link(game.id());
            } else if (index.column() == WHITE_COLUMN) {
                user = game->white();
            } else if (index.column() == BLACK_COLUMN) {
                user = game->black();
            } else if (index.column() == WINNER_COLUMN) {
                user = game->winner();
            }
            if (user) {
                return tApp->path().user_view()->get_link(user.id());
            }
        }
        t.commit();
        return GLP::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class TableView : public Wt::WTableView {
protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

class GameListWidget::GameListWidgetImpl : public Wt::WContainerWidget {
public:
    GameListWidgetImpl() :
        Wt::WContainerWidget() {
        initialize();
    }

    GameListWidgetImpl(const UserPtr& user) :
        Wt::WContainerWidget(), user_(user) {
        initialize();
    }

    void initialize() {
        manager();
        query_model_ = new GameListModel(query(), this);
        table_view_ = new TableView();
        addWidget(table_view_);
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
        std::string sql;
        sql = "select G, GP.norating, Wh.username, B.username, Wi.username "
              "from thechess_game G "
              "left join thechess_gp GP on G.gp_id=GP.id "
              "left join thechess_user Wh on G.white_id=Wh.id "
              "left join thechess_user B on G.black_id=B.id "
              "left join thechess_user Wi on G.winner_game_id=Wi.id ";
        return tApp->session().query<GLP::Result>(sql);
    }

    GLP::Q query() {
        dbo::Transaction t(tApp->session());
        GLP::Q q = all_games();
        if (only_my_->isChecked() && tApp->user()) {
            int id = tApp->user()->id();
            q.where("G.white_id = ? or G.black_id = ? or G.init_id = ?")
            .bind(id).bind(id).bind(id);
        }
        if (only_commented_->isChecked()) {
            q.where("G.name <> ''");
        }
        if (user_) {
            int id = user_->id();
            q.where("G.white_id = ? or G.black_id = ? or G.init_id = ?")
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
    Wt::WCheckBox* only_commented_;
    UserPtr user_;

    void manager() {
        only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
        only_my_->setChecked(User::has_s(SWITCH_ONLY_MY_GAMES));
        only_my_->changed().connect(this, &GameListWidgetImpl::apply);
        if (!tApp->user()) {
            only_my_->setEnabled(false);
        }
        only_commented_ = new Wt::WCheckBox(tr("tc.game.Only_commented"), this);
        only_commented_->setChecked(User::has_s(SWITCH_ONLY_COMMENTED_GAMES));
        only_commented_->changed().connect(this, &GameListWidgetImpl::apply);
        if (!tApp->environment().ajax()) {
            Wt::WPushButton* apply_button =
                new Wt::WPushButton(tr("tc.common.Apply"), this);
            apply_button->clicked().connect(this, &GameListWidgetImpl::apply);
        }
    }

    void apply() {
        User::set_s(SWITCH_ONLY_MY_GAMES, only_my_->isChecked());
        User::set_s(SWITCH_ONLY_COMMENTED_GAMES, only_commented_->isChecked());
        query_model_->setQuery(query(), /* keep_columns */ true);
    }

};

GameListWidget::GameListWidget(Wt::WContainerWidget* parent) :
    WCompositeWidget(parent) {
    impl_ = new GameListWidgetImpl();
    setImplementation(impl_);
}

GameListWidget::GameListWidget(const UserPtr& user, Wt::WContainerWidget* p):
    WCompositeWidget(p) {
    impl_ = new GameListWidgetImpl(user);
    setImplementation(impl_);
}

}

