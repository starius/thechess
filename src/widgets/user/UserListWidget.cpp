/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WTableView>
#include <Wt/WCheckBox>
#include <Wt/WPushButton>
#include <Wt/WEnvironment>
#include <Wt/Dbo/QueryModel>
#include <Wt/Wc/Pager.hpp>

#include "widgets/user/UserListWidget.hpp"
#include "Application.hpp"
#include "model/all.hpp"

namespace thechess {

namespace ULP {
typedef UserPtr Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int WIDTH = 750;
const int HEIGHT = 500;
}

class UserListModel : public ULP::BaseQM {
public:
    enum {
        NAME_COLUMN,
        CLASSIFICATION_COLUMN,
        ALL_COLUMN,
        WINS_COLUMN,
        DRAWS_COLUMN,
        FAILS_COLUMN,
        RATING_COLUMN
    };

    UserListModel(Wt::WObject* parent = 0) :
        ULP::BaseQM(parent) {
        only_online_ = User::has_s(SWITCH_ONLY_ONLINE_USERS);
        not_removed_ = User::has_s(SWITCH_ONLY_NOT_REMOVED_USERS);
        set_query();
        addColumn("id", tr("tc.common.Name")); // dummy
        addColumn("id", tr("tc.user.classification")); // dummy
        addColumn("games_stat_all", tr("tc.user.games_stat_all"));
        addColumn("games_stat_wins", tr("tc.user.games_stat_wins"));
        addColumn("id", tr("tc.user.games_stat_draws")); // dummy
        addColumn("games_stat_fails", tr("tc.user.games_stat_fails"));
        addColumn("games_stat_elo", tr("tc.user.games_stat_elo"));
        sort(RATING_COLUMN, Wt::DescendingOrder);
    }

    bool only_online() const {
        return only_online_;
    }

    void set_only_online(bool only_online) {
        only_online_ = only_online;
        User::set_s(SWITCH_ONLY_ONLINE_USERS, only_online);
        set_query();
    }

    bool not_removed() const {
        return not_removed_;
    }

    void set_not_removed(bool not_removed) {
        not_removed_ = not_removed;
        User::set_s(SWITCH_ONLY_NOT_REMOVED_USERS, not_removed);
        set_query();
    }

private:
    bool only_online_, not_removed_;

    void set_query() {
        dbo::Transaction t(tApp->session());
        ULP::Q q = tApp->session().find<User>();
        if (not_removed_) {
            q.where("rights != ?").bind(NONE);
        }
        if (only_online_) {
            q.where("sessions != 0");
        }
        setQuery(q, /* keep_columns */ true);
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const UserPtr& o = resultRow(index.row());
        if (role == Wt::DisplayRole) {
            if (index.column() == NAME_COLUMN) {
                return o->safe_username();
            } else if (index.column() == CLASSIFICATION_COLUMN) {
                return o->classification_str();
            } else if (index.column() == DRAWS_COLUMN) {
                return o->games_stat().draws();
            }
        } else if (role == Wt::LinkRole) {
            if (index.column() == NAME_COLUMN) {
                return tApp->path().user_view()->get_link(o.id());
            }
        } else if (role == Wt::StyleClassRole) {
            if (index.column() == ALL_COLUMN) {
                return "thechess-games-all";
            } else if (index.column() == WINS_COLUMN) {
                return "thechess-games-wins";
            } else if (index.column() == FAILS_COLUMN) {
                return "thechess-games-fails";
            } else if (index.column() == DRAWS_COLUMN) {
                return "thechess-games-draws";
            } else if (index.column() == RATING_COLUMN) {
                return "thechess-games-rating";
            }
        }
        t.commit();
        return ULP::BaseQM::data(index, role);
    }

    Wt::WFlags<Wt::ItemFlag> flags(const Wt::WModelIndex& i) const {
        Wt::WFlags<Wt::ItemFlag> f = ULP::BaseQM::flags(i);
        if (i.column() == NAME_COLUMN) {
            f |= Wt::ItemIsXHTMLText;
        }
        return f;
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class UserListView : public Wt::WTableView {
public:
    UserListView(UserListModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(ULP::WIDTH, ULP::HEIGHT);
        setColumnWidth(UserListModel::NAME_COLUMN, 300);
        setColumnWidth(UserListModel::CLASSIFICATION_COLUMN, 150);
        setColumnWidth(UserListModel::ALL_COLUMN, 50);
        setColumnWidth(UserListModel::WINS_COLUMN, 50);
        setColumnWidth(UserListModel::DRAWS_COLUMN, 50);
        setColumnWidth(UserListModel::FAILS_COLUMN, 50);
        setColumnWidth(UserListModel::RATING_COLUMN, 50);
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

UserListWidget::UserListWidget(Wt::WContainerWidget* parent) :
    WContainerWidget(parent) {
    UserListModel* m = new UserListModel(this);
    Wt::WCheckBox* oo = new Wt::WCheckBox(tr("tc.user.Only_online"), this);
    oo->setChecked(m->only_online());
    oo->changed().connect(
        boost::bind(&UserListModel::set_only_online, m,
                    boost::bind(&Wt::WAbstractToggleButton::isChecked, oo)));
    Wt::WCheckBox* nr = new Wt::WCheckBox(tr("tc.user.Not_removed"), this);
    nr->setChecked(m->not_removed());
    nr->changed().connect(
        boost::bind(&UserListModel::set_not_removed, m,
                    boost::bind(&Wt::WAbstractToggleButton::isChecked, nr)));
    UserListView* view = new UserListView(m, this);
}

}

