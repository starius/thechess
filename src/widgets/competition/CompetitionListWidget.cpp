/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <sstream>
#include <boost/foreach.hpp>

#include <Wt/WTableView>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>
#include <Wt/WString>
#include <Wt/WPushButton>
#include <Wt/WEnvironment>

#include "widgets/competition/CompetitionListWidget.hpp"
#include "model/all.hpp"
#include "thechess-global.hpp"
#include "Application.hpp"

namespace thechess {

namespace CLP {
typedef boost::tuple<CompetitionPtr, Wt::WString, int, int> Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;
}

class CompetitionListModel : public CLP::BaseQM {
public:
    static const int N_COLUMN = 0;
    static const int NAME_COLUMN = 1;
    static const int TYPE_COLUMN = 2;
    static const int STATE_COLUMN = 3;
    static const int WINNERS_COLUMN = 4;
    static const int MEMBERS_COLUMN = 5;
    static const int STARTED_COLUMN = 6;
    static const int ENDED_COLUMN = 7;

    CompetitionListModel(Wt::WObject* parent = 0) :
        CLP::BaseQM(parent) {
        set_query();
        addColumn("C.id", tr("tc.common.number"));
        addColumn("C.name", tr("tc.common.Name"));
        addColumn("CP.type", tr("tc.competition.Type"));
        addColumn("C.state", tr("tc.competition.State"));
        addColumn("C.id", tr("tc.competition.Winners"));
        addColumn("C.id", tr("tc.competition.members_number"));
        addColumn("C.started", tr("tc.competition.Started"));
        addColumn("C.ended", tr("tc.competition.Ended"));
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const CompetitionPtr& o = resultRow(index.row()).get<0>();
        if (role == Wt::DisplayRole) {
            if (index.column() == TYPE_COLUMN) {
                return Competition::type2str(o->type());
            } else if (index.column() == STATE_COLUMN) {
                return tr(Competition::state2str(o->state()));
            } else if (index.column() == WINNERS_COLUMN) {
                return resultRow(index.row()).get<1>();
            } else if (index.column() == MEMBERS_COLUMN) {
                return resultRow(index.row()).get<2>();
            }
        } else if (role == Wt::LinkRole && index.column() == N_COLUMN) {
            return tApp->path().competition_view()->get_link(o.id());
        }
        return CLP::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }

    void set_query(bool only_my = false) {
        DatabaseType t = tApp->server().options().database_type();
        std::stringstream sql;
        sql << "select C, ";
        if (t == POSTGRES) {
            sql << "array_to_string(array_agg(distinct W.username), ', '), ";
        } else if (t == SQLITE3) {
            sql << "group_concat(distinct W.username), ";
        }
        sql << "count(distinct M.thechess_user_id), ";
        sql << "CP.type ";
        if (only_my)
            sql << "from members_competitions I "
                "left join thechess_competition C "
                "on C.id=I.thechess_competition_id ";
        else {
            sql << "from thechess_competition C ";
        }
        sql << "left join winners_competition WC "
            "on WC.thechess_competition_id=C.id ";
        sql << "left join thechess_user W on WC.thechess_user_id=W.id ";
        sql << "left join members_competitions M "
            "on M.thechess_competition_id=C.id ";
        sql << "left join thechess_cp CP on C.cp_id=CP.id ";
        if (only_my) {
            sql << "where I.thechess_user_id = ? ";
        }
        CLP::Q q = tApp->session().query<CLP::Result>(sql.str());
        q.groupBy("C");
        if (only_my) {
            q.bind(tApp->user().id());
        }
        setQuery(q, /* keep_columns */ true);
    }
};

class CompetitionListView : public Wt::WTableView {
public:
    CompetitionListView(CompetitionListModel* model,
                        Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        setColumnWidth(CompetitionListModel::N_COLUMN, 40);
        setColumnWidth(CompetitionListModel::NAME_COLUMN, 170);
        setColumnWidth(CompetitionListModel::TYPE_COLUMN, 75);
        setColumnWidth(CompetitionListModel::STATE_COLUMN, 75);
        setColumnWidth(CompetitionListModel::WINNERS_COLUMN, 160);
        setColumnWidth(CompetitionListModel::MEMBERS_COLUMN, 50);
        setColumnWidth(CompetitionListModel::STARTED_COLUMN, 70);
        setColumnWidth(CompetitionListModel::ENDED_COLUMN, 70);
        setRowHeaderCount(1); // fixed id_columns when scrolling
    }
};

CompetitionListWidget::CompetitionListWidget(Wt::WContainerWidget* p):
    Wt::WContainerWidget(p) {
    Wt::WVBoxLayout* layout = new Wt::WVBoxLayout(this);
    Wt::WHBoxLayout* manager_layout = new Wt::WHBoxLayout();
    layout->addLayout(manager_layout);
    manager(manager_layout);
    model_ = new CompetitionListModel(this);
    apply();
    CompetitionListView* view = new CompetitionListView(model_);
    layout->addWidget(view);
    layout->setStretchFactor(view, 1);
}

void CompetitionListWidget::apply() {
    bool only_my = only_my_->isChecked() && tApp->user();
    model_->set_query(only_my);
}

void CompetitionListWidget::manager(Wt::WHBoxLayout* layout) {
    only_my_ = new Wt::WCheckBox(tr("tc.common.Only_my"));
    layout->addWidget(only_my_);
    only_my_->changed().connect(this, &CompetitionListWidget::apply);
    if (!tApp->user()) {
        only_my_->setEnabled(false);
    }
    if (!tApp->environment().ajax()) {
        Wt::WPushButton* apply_button =
            new Wt::WPushButton(tr("tc.common.Apply"));
        layout->addWidget(apply_button);
        apply_button->clicked().connect(this, &CompetitionListWidget::apply);
    }
    layout->addStretch(1);
}

}

