/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WEnvironment>
#include <Wt/WTableView>
#include <Wt/WCheckBox>
#include <Wt/WPushButton>
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
        RATING_COLUMN,
        ONLINE_TIME,
        REGISTRATION_DATE
    };

    UserListModel(Wt::WObject* parent = 0) :
        ULP::BaseQM(parent),
        only_blocked_(false) {
        only_online_ = User::has_s(SWITCH_ONLY_ONLINE_USERS);
        not_removed_ = User::has_s(SWITCH_ONLY_NOT_REMOVED_USERS);
        set_query();
        addColumn("username", tr("tc.common.Name")); // dummy
        addColumn("classification", tr("tc.user.classification")); // dummy
        addColumn("games_stat_all", tr("tc.user.games_stat_all"));
        addColumn("games_stat_wins", tr("tc.user.games_stat_wins"));
        addColumn("id", tr("tc.user.games_stat_draws")); // dummy
        addColumn("games_stat_fails", tr("tc.user.games_stat_fails"));
        addColumn("games_stat_elo", tr("tc.user.games_stat_elo"));
        addColumn("online_time", tr("tc.user.Online_time").arg("")); // dummy
        addColumn("registration_date", tr("tc.user.Registration_date").arg(""));
    }

    bool only_online() const {
        return only_online_;
    }

    void set_only_online(bool only_online) {
        if (only_online != only_online_) {
            only_online_ = only_online;
            User::set_s(SWITCH_ONLY_ONLINE_USERS, only_online);
            set_query();
        }
    }

    bool not_removed() const {
        return not_removed_;
    }

    void set_not_removed(bool not_removed) {
        if (not_removed != not_removed_) {
            not_removed_ = not_removed;
            User::set_s(SWITCH_ONLY_NOT_REMOVED_USERS, not_removed);
            set_query();
        }
    }

    bool only_blocked() const {
        return only_blocked_;
    }

    void set_only_blocked(bool only_blocked) {
        if (only_blocked != only_blocked_) {
            only_blocked_ = only_blocked;
            set_query();
        }
    }

private:
    bool only_online_, not_removed_, only_blocked_;

    void set_query() {
        dbo::Transaction t(tApp->session());
        ULP::Q q;
        if (only_blocked_) {
            q = tApp->session().query<UserPtr>(
                    "select U from thechess_message_filter F "
                    "left join thechess_user U on F.bad_id = U.id");
            q.where("F.good_id = ?").bind(tApp->user());
        } else {
            q = tApp->session().find<User>();
        }
        if (not_removed_) {
            q.where("rights != ?").bind(NONE);
            q.where("vacation_until is null");
            q.where("last_enter > ?").bind(now() - 10 * WEEK);
        }
        if (only_online_) {
            q.where("sessions != 0");
        }
        q.orderBy("games_stat_elo desc");
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
            } else if (index.column() == ONLINE_TIME) {
                return td2str(o->online_time());
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
            } else if (index.column() == ONLINE_TIME ||
                       index.column() == REGISTRATION_DATE) {
                return "thechess-datetime";
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
        setColumnWidth(UserListModel::NAME_COLUMN, 170);
        setColumnWidth(UserListModel::CLASSIFICATION_COLUMN, 150);
        setColumnWidth(UserListModel::ALL_COLUMN, 40);
        setColumnWidth(UserListModel::WINS_COLUMN, 40);
        setColumnWidth(UserListModel::DRAWS_COLUMN, 30);
        setColumnWidth(UserListModel::FAILS_COLUMN, 40);
        setColumnWidth(UserListModel::RATING_COLUMN, 50);
        setColumnWidth(UserListModel::ONLINE_TIME, 85);
        setColumnWidth(UserListModel::REGISTRATION_DATE, 75);
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

UserListWidget::UserListWidget(Wt::WContainerWidget* parent) :
    WContainerWidget(parent), b_(0) {
    m_ = new UserListModel(this);
    oo_ = new Wt::WCheckBox(tr("tc.user.Only_online"), this);
    oo_->setChecked(m_->only_online());
    oo_->changed().connect(this, &UserListWidget::apply);
    nr_ = new Wt::WCheckBox(tr("tc.user.Not_removed"), this);
    nr_->setChecked(m_->not_removed());
    nr_->changed().connect(this, &UserListWidget::apply);
    if (tApp->user()) {
        b_ = new Wt::WCheckBox(tr("tc.user.My_blocklist"), this);
        b_->setChecked(m_->only_blocked());
        b_->changed().connect(this, &UserListWidget::apply);
    }
    if (!wApp->environment().ajax()) {
        Wt::WPushButton* b = new Wt::WPushButton(tr("tc.common.Apply"), this);
        b->clicked().connect(this, &UserListWidget::apply);
    }
    UserListView* view = new UserListView(m_, this);
}

void UserListWidget::apply() {
    m_->set_only_online(oo_->isChecked());
    m_->set_not_removed(nr_->isChecked());
    if (b_) {
        m_->set_only_blocked(b_->isChecked());
    }
}

}

