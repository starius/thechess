/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <sstream>
#include <boost/foreach.hpp>

#include <Wt/WTableView>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>
#include <Wt/WString>
#include <Wt/WPushButton>
#include <Wt/WEnvironment>

#include "widgets/CompetitionListWidget.hpp"
#include "model/Competition.hpp"
#include "model/User.hpp"
#include "ThechessApplication.hpp"

namespace thechess {

typedef CompetitionListWidget::Result Result;
typedef CompetitionListWidget::Q Q;
typedef dbo::QueryModel<Result> BaseQM;

const int n_column = 0;
const int name_column = 1;
const int type_column = 2;
const int state_column = 3;
const int winners_column = 4;
const int members_column = 5;
const int started_column = 6;
const int ended_column = 7;

class CompetitionListModel : public BaseQM {
public:
    CompetitionListModel(const Q& query, Wt::WObject* parent = 0) :
        BaseQM(parent) {
        setQuery(query);
        addColumn("C.id", tr("tc.common.number"));
        addColumn("C.name", tr("tc.competition.Name"));
        addColumn("C.type", tr("tc.competition.Type"));
        addColumn("C.state", tr("tc.competition.State"));
        addColumn("C.id", tr("tc.competition.Winners"));
        addColumn("C.id", tr("tc.competition.members_number"));
        addColumn("C.started", tr("tc.competition.Started"));
        addColumn("C.ended", tr("tc.competition.Ended"));
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        CompetitionPtr o = resultRow(index.row()).get<0>();
        if (role == Wt::DisplayRole) {
            if (index.column() == type_column) {
                return Competition::type2str(o->type());
            } else if (index.column() == state_column) {
                return tr(Competition::state2str(o->state()));
            } else if (index.column() == winners_column) {
                return resultRow(index.row()).get<1>();
            } else if (index.column() == members_column) {
                return resultRow(index.row()).get<2>();
            }
        } else if (role == Wt::InternalPathRole && index.column() == n_column) {
            return str(boost::format("/competition/%i/") % o.id());
        }
        return BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class CompetitionListView : public Wt::WTableView {
public:
    CompetitionListView(CompetitionListModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(770, 450);
        setColumnWidth(n_column, 40);
        setColumnWidth(name_column, 170);
        setColumnWidth(type_column, 75);
        setColumnWidth(state_column, 75);
        setColumnWidth(winners_column, 160);
        setColumnWidth(members_column, 50);
        setColumnWidth(started_column, 70);
        setColumnWidth(ended_column, 70);
        setRowHeaderCount(1); // fixed id_columns when scrolling
    }
};

CompetitionListWidget::CompetitionListWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    manager_();
    model_ = new CompetitionListModel(query_(), this);
    new CompetitionListView(model_, this);
}

Q CompetitionListWidget::query_() {
    bool only_my = only_my_->isChecked() && tApp->user();
    ThechessOptions::DatabaseType t = tApp->server().options().database_type();
    std::stringstream sql;
    sql << "select C, ";
    if (t == ThechessOptions::Postgres) {
        sql << "array_to_string(array_agg(distinct W.username), ', '), ";
    } else if (t == ThechessOptions::Sqlite3) {
        sql << "group_concat(distinct W.username), ";
    }
    sql << "count(distinct M.thechess_user_id) ";
    if (only_my)
        sql << "from members_competitions I left join thechess_competition C "
            "on C.id=I.thechess_competition_id ";
    else {
        sql << "from thechess_competition C ";
    }
    sql << "left join winners_competition WC on WC.thechess_competition_id=C.id ";
    sql << "left join thechess_user W on WC.thechess_user_id=W.id ";
    sql << "left join members_competitions M on M.thechess_competition_id=C.id ";
    if (only_my) {
        sql << "where I.thechess_user_id = ?";
    }
    Q q = tApp->session().query<Result>(sql.str());
    q.groupBy("C");
    if (only_my) {
        q.bind(tApp->user().id());
    }
    return q;
}

void CompetitionListWidget::apply_() {
    model_->setQuery(query_(), /* keep_columns */ true);
}

void CompetitionListWidget::manager_() {
    only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"), this);
    only_my_->changed().connect(this, &CompetitionListWidget::apply_);
    if (!tApp->user()) {
        only_my_->setEnabled(false);
    }
    if (!tApp->environment().ajax()) {
        Wt::WPushButton* apply_button =
            new Wt::WPushButton(tr("tc.common.Apply"), this);
        apply_button->clicked().connect(this, &CompetitionListWidget::apply_);
    }
}

}

