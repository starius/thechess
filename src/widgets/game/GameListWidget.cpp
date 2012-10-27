/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <sstream>
#include <boost/format.hpp>
#include <boost/tuple/tuple.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WTableView>
#include <Wt/WAnchor>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>
#include <Wt/Dbo/ptr_tuple>
#include <Wt/Dbo/ptr>
#include <Wt/Wc/Pager.hpp>
#include <Wt/Wc/util.hpp>

#include <Wt/WCheckBox>
#include <Wt/WPushButton>
#include <Wt/WString>
#include <Wt/WEnvironment>

#include "widgets/game/GameListWidget.hpp"
#include "widgets/Header.hpp"
#include "Application.hpp"
#include "SharedBoardIndex.hpp"
#include "model/all.hpp"

namespace thechess {

namespace GLP {
typedef Wt::WString S;
typedef boost::tuple<GamePtr> Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int GAME_IN_TUPLE = 0;
}

class GameStateSelect : public Wt::WComboBox {
public:
    enum State {
        ALL,
        NOTSTARTED,
        ACTIVE,
        PAUSE,
        ENDED,
        WHITE_WON,
        BLACK_WON,
        DRAW,
        CANCELLED
    };

    GameStateSelect(Wt::WContainerWidget* parent = 0):
        WComboBox(parent) {
        addItem(tr("tc.common.all"));
        addItem(tr("tc.common.not_started"));
        addItem(tr(Game::state2str_id(Game::ACTIVE)));
        addItem(tr(Game::state2str_id(Game::PAUSE)));
        addItem(tr("tc.game.ended"));
        addItem(tr("tc.game.White_won"));
        addItem(tr("tc.game.Black_won"));
        addItem(tr("tc.common.Draws"));
        addItem(tr(Game::state2str_id(Game::CANCELLED)));
    }

    State state() const {
        return State(currentIndex());
    }

    static Game::State state(State s) {
        if (s == ACTIVE) {
            return Game::ACTIVE;
        } else if (s == PAUSE) {
            return Game::PAUSE;
        } else if (s == CANCELLED) {
            return Game::CANCELLED;
        }
        return Game::CANCELLED; // unknown state
    }
};

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
        user_(user),
        state_(GameStateSelect::ALL) {
        set_query();
        addColumn("G.id", tr("tc.common.number"));
        addColumn("G.id", tr("tc.game.white")); // dummy
        addColumn("G.id", tr("tc.game.black")); // dummy
        addColumn("G.state", tr("tc.game.State")); // dummy
        addColumn("G.id", tr("tc.common.winner")); // dummy
        addColumn("G.started", tr("tc.game.started"));
        addColumn("G.id", tr("tc.game.moves_size")); // dummy
        addColumn("G.name", tr("tc.game.comment"));
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const GamePtr& game = resultRow(index.row()).get<GLP::GAME_IN_TUPLE>();
        if (role == Wt::DisplayRole) {
            if (index.column() == WHITE_COLUMN) {
                return game->white() ? game->white()->safe_username() : "";
            } else if (index.column() == BLACK_COLUMN) {
                return game->black() ? game->black()->safe_username() : "";
            } else if (index.column() == WINNER_COLUMN) {
                return game->winner() ? game->winner()->safe_username() : "";
            } else if (index.column() == STATE_COLUMN) {
                return game->str_state();
            } else if (index.column() == MOVES_SIZE_COLUMN) {
                return game->moves_number();
            }
        } else if (role == Wt::StyleClassRole) {
            if (index.column() == WHITE_COLUMN ||
                    index.column() == BLACK_COLUMN ||
                    index.column() == WINNER_COLUMN) {
                return "thechess-user-anchor";
            } else if (index.column() == STARTED_COLUMN) {
                return "thechess-datetime";
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

    Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex& i) const {
        Wt::WFlags<Wt::ItemFlag> f = GLP::BaseQM::flags(i);
        if (i.column() == WHITE_COLUMN || i.column() == BLACK_COLUMN ||
                i.column() == WINNER_COLUMN || i.column() == COMMENT_COLUMN) {
            f |= Wt::ItemIsXHTMLText;
        }
        return f;
    }

    void set_query() {
        dbo::Transaction t(tApp->session());
        GLP::Q q = tApp->session().query<GLP::Result>(
                       "select G from thechess_game G");
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
        if (competition_) {
            q.where("G.competition_id = ?").bind(competition_);
        }
        if (!games_.empty()) {
            std::stringstream ids;
            ids << "G.id in (";
            for (int i = 0; i < games_.size(); i++) {
                if (i != 0) {
                    ids << ',';
                }
                ids << games_[i];
            }
            ids << ')';
            q.where(ids.str());
        }
        if (state_ == GameStateSelect::NOTSTARTED) {
            q.where("G.state < ?").bind(Game::ACTIVE);
        } else if (state_ == GameStateSelect::ENDED) {
            q.where("G.state >= ?").bind(Game::MIN_ENDED);
        } else if (state_ == GameStateSelect::WHITE_WON) {
            q.where("G.state >= ?").bind(Game::MIN_ENDED);
            q.where("G.white_id = G.winner_game_id");
        } else if (state_ == GameStateSelect::BLACK_WON) {
            q.where("G.state >= ?").bind(Game::MIN_ENDED);
            q.where("G.black_id = G.winner_game_id");
        } else if (state_ == GameStateSelect::DRAW) {
            q.where("G.state >= ? and G.state <= ?");
            q.bind(Game::MIN_DRAW).bind(Game::MAX_DRAW);
        } else if (state_ != GameStateSelect::ALL) {
            q.where("G.state = ?").bind(GameStateSelect::state(state_));
        }
        if (!name_like_.empty()) {
            q.where("(G.name like ? or G.id = ?)");
            q.bind("%" + name_like_ + "%");
            q.bind(Wt::Wc::str2int(name_like_.toUTF8()));
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

    void set_state(GameStateSelect::State state) {
        if (state != state_) {
            state_ = state;
            set_query();
        }
    }

    void set_name_like(const Wt::WString& name_like) {
        if (name_like != name_like_) {
            name_like_ = name_like;
            set_query();
        }
    }

    void set_competition(const CompetitionPtr& competition) {
        if (competition != competition_) {
            competition_ = competition;
            set_query();
        }
    }

    void set_games(const std::vector<int>& games) {
        games_ = games;
        set_query();
    }

private:
    bool only_my_;
    bool only_commented_;
    UserPtr user_;
    CompetitionPtr competition_;
    std::vector<int> games_;
    GameStateSelect::State state_;
    Wt::WString name_like_;
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

static const std::vector<int> NO_GAMES(1, -1);

class GameListWidgetImpl : public Wt::WContainerWidget {
public:
    GameListWidgetImpl() {
        initialize();
    }

    GameListWidgetImpl(const UserPtr& user) {
        initialize(user);
    }

    GameListWidgetImpl(const CompetitionPtr& competition) {
        initialize(UserPtr(), competition);
    }

    GameListWidgetImpl(const std::vector<int>& games) {
        const std::vector<int>& g = games.empty() ? NO_GAMES : games;
        initialize(UserPtr(), CompetitionPtr(), g);
    }

    GameListWidgetImpl(const Board& b) {
        std::vector<int> games;
        SharedBoardIndex::instance()->search_board(tApp->session(), b, games);
        const std::vector<int>& g = games.empty() ? NO_GAMES : games;
        initialize(UserPtr(), CompetitionPtr(), g);
    }

    GameListWidgetImpl(const Moves& m) {
        std::vector<int> games;
        SharedBoardIndex::instance()->search_moves(tApp->session(), m, games);
        const std::vector<int>& g = games.empty() ? NO_GAMES : games;
        initialize(UserPtr(), CompetitionPtr(), g);
    }

    void initialize(const UserPtr& user = UserPtr(),
                    const CompetitionPtr& c = CompetitionPtr(),
                    const std::vector<int>& games = std::vector<int>()) {
        model_ = new GameListModel(user, this);
        if (c) {
            model_->set_competition(c);
        }
        if (!games.empty()) {
            model_->set_games(games);
        }
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
        Wt::Wc::scroll_to_last(table_view_);
    }

private:
    GameListModel* model_;
    Wt::WTableView* table_view_;
    Wt::WCheckBox* only_my_;
    Wt::WCheckBox* only_commented_;
    GameStateSelect* state_;
    Wt::WLineEdit* name_like_;

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
        state_ = new GameStateSelect(this);
        state_->changed().connect(this, &GameListWidgetImpl::apply);
        name_like_ = new Wt::WLineEdit(this);
        name_like_->setEmptyText(tr("tc.game.comment"));
        name_like_->enterPressed().connect(this, &GameListWidgetImpl::apply);
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
        model_->set_state(state_->state());
        model_->set_name_like(name_like_->text());
        Wt::Wc::scroll_to_last(table_view_);
    }

};

GameListWidget::GameListWidget(Wt::WContainerWidget* parent) :
    WContainerWidget(parent) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl());
    //addWidget(new Wt::WAnchor("/pgn/all.pgn", tr("tc.game.Download_pgn")));
}

GameListWidget::GameListWidget(const UserPtr& user, Wt::WContainerWidget* p):
    WContainerWidget(p) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl(user));
}

GameListWidget::GameListWidget(const CompetitionPtr& competition,
                               Wt::WContainerWidget* p):
    WContainerWidget(p) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl(competition));
}

GameListWidget::GameListWidget(const std::vector<int>& games,
                               Wt::WContainerWidget* p):
    WContainerWidget(p) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl(games));
}

GameListWidget::GameListWidget(const Board& board, Wt::WContainerWidget* p):
    WContainerWidget(p) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl(board));
}

GameListWidget::GameListWidget(const Moves& moves, Wt::WContainerWidget* p):
    WContainerWidget(p) {
    addWidget(new Header(tr("tc.game.List")));
    addWidget(new GameListWidgetImpl(moves));
}

}

