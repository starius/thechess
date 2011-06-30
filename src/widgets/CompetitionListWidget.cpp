
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
namespace widgets {
using namespace model;

typedef CompetitionListWidget::Result Result;
typedef CompetitionListWidget::Q Q;
typedef dbo::QueryModel<Result> BaseQM;

const int n_column = 0;
const int name_column = 1;
const int type_column = 2;
const int state_column = 3;
const int winners_column = 4;
const int started_column = 5;
const int ended_column = 6;

class CompetitionListModel : public BaseQM
{
public:
    CompetitionListModel(const Q& query, Wt::WObject *parent=0) :
    BaseQM(parent)
    {
        setQuery(query);
        addColumn("C.id", tr("thechess.number"));
        addColumn("C.name", tr("thechess.competition.name"));
        addColumn("C.type", tr("thechess.competition.type"));
        addColumn("C.state", tr("thechess.competition.state"));
        addColumn("C.id", tr("thechess.competition.winners"));
        addColumn("C.started", tr("thechess.competition.started"));
        addColumn("C.ended", tr("thechess.competition.ended"));
    }

    boost::any data(const Wt::WModelIndex& index,
        int role=Wt::DisplayRole) const
    {
        dbo::Transaction t(tApp->session());
        CompetitionPtr o = resultRow(index.row()).get<0>();
        if (role == Wt::DisplayRole)
        {
            if (index.column() == type_column)
                return Competition::type2str(o->type());
            else if (index.column() == state_column)
                return tr(Competition::state2str(o->state()));
            else if (index.column() == winners_column)
                return resultRow(index.row()).get<1>();
        }
        else if (role == Wt::InternalPathRole && index.column() == n_column)
            return str(boost::format("/competition/%i/") % o.id());
        return BaseQM::data(index, role);
    }

    static Wt::WString tr(const char* key)
    {
        return Wt::WString::tr(key);
    }
};

class CompetitionListView : public Wt::WTableView
{
public:
    CompetitionListView(CompetitionListModel* model, Wt::WContainerWidget* p=0):
    Wt::WTableView(p)
    {
        setModel(model);
        resize(831, 450);
        setColumnWidth(n_column, 40);
        setColumnWidth(name_column, 200);
        setColumnWidth(type_column, 100);
        setColumnWidth(state_column, 110);
        setColumnWidth(winners_column, 90);
        setColumnWidth(started_column, 120);
        setColumnWidth(ended_column, 120);
        setRowHeaderCount(1); // fixed id_columns when scrolling
    }
};

CompetitionListWidget::CompetitionListWidget(Wt::WContainerWidget* p):
Wt::WContainerWidget(p)
{
    model_ = new CompetitionListModel(query_(), this);
    new CompetitionListView(model_, this);
}

Q CompetitionListWidget::query_()
{
    ThechessOptions::DatabaseType t = tApp->server().options().database_type();
    std::stringstream sql;
    sql << "select C, ";
    if(t == ThechessOptions::Postgres)
        sql << "array_to_string(array_agg(distinct W.username), ', ') ";
    else if(t == ThechessOptions::Sqlite3)
        sql << "group_concat(W.username, ', ') ";
    sql << "from thechess_competition C ";
    sql << "left join winners_competition WC on WC.thechess_competition_id=C.id ";
    sql << "left join thechess_user W on WC.thechess_user_id=W.id ";
    Q q = tApp->session().query<Result>(sql.str());
    q.groupBy("C");
    return q;
}

}
}

