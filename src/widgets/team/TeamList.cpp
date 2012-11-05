/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/bind.hpp>

#include <Wt/WTableView>
#include <Wt/Dbo/QueryModel>
#include <Wt/Wc/Pager.hpp>

#include "widgets/team/TeamList.hpp"
#include "model/all.hpp"

namespace thechess {

namespace TLP {
typedef TeamPtr Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;
}

class TeamListModel : public TLP::BaseQM {
public:
    enum {
        ID_COL,
        NAME_COL,
        DESCRIPTION_COL
    };

    TeamListModel(Wt::WObject* parent = 0) :
        TLP::BaseQM(parent) {
        set_query();
        addColumn("id", tr("tc.common.number"));
        addColumn("name", tr("tc.common.Name"));
        addColumn("description", tr("tc.common.Description"));
    }

private:
    void set_query() {
        dbo::Transaction t(tApp->session());
        TLP::Q q;
        q = tApp->session().query<TeamPtr>("select * from thechess_team");
        setQuery(q, /* keep_columns */ true);
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        dbo::Transaction t(tApp->session());
        const TeamPtr& o = resultRow(index.row());
        if (role == Wt::LinkRole) {
            if (index.column() == ID_COL) {
                // TODO
            }
        }
        t.commit();
        return TLP::BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key) {
        return Wt::WString::tr(key);
    }
};

class TeamListView : public Wt::WTableView {
public:
    TeamListView(TeamListModel* model, Wt::WContainerWidget* p = 0):
        Wt::WTableView(p) {
        setModel(model);
        resize(770, 450);
        setColumnWidth(TeamListModel::ID_COL, 40);
        setColumnWidth(TeamListModel::NAME_COL, 170);
        setColumnWidth(TeamListModel::DESCRIPTION_COL, 770 - 40 - 170);
        setSelectable(true);
    }

protected:
    WWidget* createPageNavigationBar() {
        return new Wt::Wc::Pager(this);
    }
};

TeamList::TeamList() {
    TeamListModel* m = new TeamListModel(this);
    TeamListView* view = new TeamListView(m, this);
    // TODO team creation button
}

}

