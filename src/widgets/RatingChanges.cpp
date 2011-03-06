
#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>
namespace dbo = Wt::Dbo;
#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
#include <Wt/WTableView>
#include <Wt/WDate>
#include <Wt/WEnvironment>
#include <Wt/WApplication>

#include "widgets/RatingChanges.hpp"
#include "model/Game.hpp"
#include "model/User.hpp"
#include "ThechessApplication.hpp"

namespace thechess {
namespace widgets {

using model::GamePtr;
using model::Game;
using model::UserPtr;

typedef GamePtr Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

const int id_column = 0;
const int ended_column = 1;
const int rating_after_column = 2;

class RatingModel : public BaseQM
{
public:
    RatingModel(UserPtr user, Wt::WObject *parent=0) :
    BaseQM(parent), user_(user)
    {
        dbo::Transaction t(tApp->session());
        dbo::Query<Result> query = tApp->session().find<Game>();
        query.where("white_id = ? or black_id = ?").bind(user_.id()).bind(user_.id());
        query.where("rating_after_white != -1");
        query.orderBy("ended");
        setQuery(query);
        addColumn("id", Wt::WString::tr("thechess.number"));
        addColumn("ended", Wt::WString::tr("thechess.ended"));
        addColumn("rating_after_white",
            Wt::WString::tr("thechess.rating_after"));
        t.commit();
    }

    boost::any data(const Wt::WModelIndex& index, 
        int role=Wt::DisplayRole) const
    {
        if (index.column() == ended_column && role == Wt::DisplayRole)
        {
            GamePtr game = resultRow(index.row());
            return Wt::WDate(game->ended().date());
        }
        if (index.column() == rating_after_column && role == Wt::DisplayRole)
        {
            GamePtr game = resultRow(index.row());
            return game->rating_after(game->color_of(user_));
        }
        else
        {
            return BaseQM::data(index, role);
        }
    }

private:
    const UserPtr user_;
};

class MultiRatingModel : public Wt::WAbstractTableModel
{
public:
    void add_user(UserPtr user)
    {
    }

private:
};

class RatingChangesImpl : public Wt::WContainerWidget
{
public:
    RatingChangesImpl(UserPtr user) :
    Wt::WContainerWidget()
    {
        RatingModel* rating_model_ = new RatingModel(user, this);
        Wt::Chart::WCartesianChart* chart_ =
            new Wt::Chart::WCartesianChart(Wt::Chart::ScatterPlot, this);
        chart_->setModel(rating_model_);
        chart_->setXSeriesColumn(ended_column);
        Wt::Chart::WDataSeries series(rating_after_column, Wt::Chart::CurveSeries);
        series.setMarker(Wt::Chart::CircleMarker);
        chart_->addSeries(series);
        chart_->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateScale);
        chart_->resize(400, 200);
        if (!wApp->environment().ajax())
        {
            chart_->setPreferredMethod(Wt::WPaintedWidget::PngImage);
        }

        Wt::WTableView* table_view_ = new Wt::WTableView(this);
        table_view_->setModel(rating_model_);
        table_view_->resize(500, 200);
    }
};


RatingChanges::RatingChanges(UserPtr user, Wt::WContainerWidget* parent):
Wt::WCompositeWidget(parent)
{
    impl_ = new RatingChangesImpl(user);
    setImplementation(impl_);
}

}
}

