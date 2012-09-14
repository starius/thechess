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
#include <Wt/WAnchor>
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
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "model/all.hpp"

namespace thechess {

namespace GLP {
typedef Wt::WString S;
typedef boost::tuple<GamePtr, S, S, S> Result;
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
        MOVES_SIZE_COLUMN,
        COMMENT_COLUMN
    };

    GameListModel(const UserPtr& user, Wt::WObject* parent = 0) :
        GLP::BaseQM(parent),
        only_my_(false),
        only_commented_(false),
        user_(user) {
        set_query();
        addColumn("G.id", tr("tc.common.number"));
        addColumn("Wh.username", tr("tc.game.white"));
        addColumn("B.username", tr("tc.game.black"));
        addColumn("G.state", tr("tc.game.State")); // dummy
        addColumn("Wi.username", tr("tc.common.winner"));
        addColumn("G.started", tr("tc.game.started"));
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

    void set_query() {
        dbo::Transaction t(tApp->session());
        GLP::Q q = tApp->session().query<GLP::Result>(
                       "select G, Wh.username, B.username, Wi.username "
                       "from thechess_game G "
                       "left join thechess_user Wh on G.white_id=Wh.id "
                       "left join thechess_user B on G.black_id=B.id "
                       "left join thechess_user Wi on G.winner_game_id=Wi.id ");
        if (only_my_ && tApp->user()) {
            int id = tApp->user()->id();
            q.where("G.white_id = ? or G.black_id = ? or G.init_id = ?")
            .bind(id).bind(id).bind(id);
        }
        if (only_commented_) {
            q.where("G.name <> ''");
        }
        if (user_) {
            int id = user_->id();
            q.where("G.white_id = ? or G.black_id = ? or G.init_id = ?")
            .bind(id).bind(id).bind(id);
        }
        q.orderBy("G.id");
        setQuery(q, /* keep_columns */ true);
        t.commit();
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }

    void set_only_my(bool only_my) {
        if (only_my != only_my_) {
            only_my_ = only_my;
            set_query();
        }
    }

    void set_only_commented(bool only_commented) {
        if (only_commented != only_commented_) {
            only_commented_ = only_commented;
            set_query();
        }
    }

private:
    bool only_my_;
    bool only_commented_;
    UserPtr user_;
};

class GameTableView : public Wt::WTableView {
protected:
    WWidget* createPageNavigationBar() {
        Wt::Wc::Pager* pager = new Wt::Wc::Pager(this);
        pager->set_base(10);
        pager->set_half_of_frame(2);
        return pager;
    }
};

class GameListWidgetImpl : public Wt::WContainerWidget {
public:
    GameListWidgetImpl() :
        Wt::WContainerWidget() {
        initialize();
    }

    GameListWidgetImpl(const UserPtr& user) :
        Wt::WContainerWidget() {
        initialize(user);
    }

    void initialize(const UserPtr& user = UserPtr()) {
        model_ = new GameListModel(user, this);
        manager();
        table_view_ = new GameTableView();
        addWidget(table_view_);
        table_view_->setModel(model_);
        table_view_->setSortingEnabled(false);
        table_view_->resize(770, 450);
        table_view_->setColumnWidth(GameListModel::N_COLUMN, 65);
        table_view_->setColumnWidth(GameListModel::WHITE_COLUMN, 75);
        table_view_->setColumnWidth(GameListModel::BLACK_COLUMN, 75);
        table_view_->setColumnWidth(GameListModel::STATE_COLUMN, 80);
        table_view_->setColumnWidth(GameListModel::WINNER_COLUMN, 120);
        table_view_->setColumnWidth(GameListModel::STARTED_COLUMN, 70);
        table_view_->setColumnWidth(GameListModel::MOVES_SIZE_COLUMN, 40);
        table_view_->setColumnWidth(GameListModel::COMMENT_COLUMN, 120);
    }

private:
    GameListModel* model_;
    Wt::WTableView* table_view_;
    Wt::WCheckBox* only_my_;
    Wt::WCheckBox* only_commented_;

    void manager() {
        only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
        only_my_->setChecked(User::has_s(SWITCH_ONLY_MY_GAMES));
        model_->set_only_my(User::has_s(SWITCH_ONLY_MY_GAMES));
        only_my_->changed().connect(this, &GameListWidgetImpl::apply);
        if (!tApp->user()) {
            only_my_->setEnabled(false);
        }
        only_commented_ = new Wt::WCheckBox(tr("tc.game.Only_commented"), this);
        only_commented_->setChecked(User::has_s(SWITCH_ONLY_COMMENTED_GAMES));
        model_->set_only_commented(User::has_s(SWITCH_ONLY_COMMENTED_GAMES));
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
        model_->set_only_my(only_my_->isChecked());
        model_->set_only_commented(only_commented_->isChecked());
    }

};

GameListWidget::GameListWidget(Wt::WContainerWidget* parent) :
    WContainerWidget(parent) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl());
    addWidget(new Wt::WAnchor("/pgn/all.pgn", tr("tc.game.Download_pgn")));
}

GameListWidget::GameListWidget(const UserPtr& user, Wt::WContainerWidget* p):
    WContainerWidget(p) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl(user));
}

}

