
#include <utility>
#include <vector>
#include <boost/foreach.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>
namespace dbo = Wt::Dbo;
#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
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

const int ID_COLUMN = 0;
const int ENDED_COLUMN = 1;
const int RATING_AFTER_COLUMN = 2;
const int COLUMNS = 3;

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
        if (index.column() == ENDED_COLUMN && role == Wt::DisplayRole)
        {
            GamePtr game = resultRow(index.row());
            return Wt::WDate(game->ended().date());
        }
        if (index.column() == RATING_AFTER_COLUMN && role == Wt::DisplayRole)
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
    MultiRatingModel(Wt::WObject* parent=0):
    Wt::WAbstractTableModel(parent), column_count_(0), row_count_(0)
    {
    }

    void add_user(UserPtr user)
    {
        RatingModel* model = new RatingModel(user, this);
        models_.push_back(model);
        column_count_ += COLUMNS;
        row_count_ = std::max(row_count_, model->rowCount());
    }

    virtual int columnCount(const Wt::WModelIndex& /* parent */) const
    {
        return column_count_;
    }

    virtual int rowCount(const Wt::WModelIndex& /* parent */) const
    {
        return row_count_;
    }

    boost::any data(const Wt::WModelIndex& index,
        int role=Wt::DisplayRole) const
    {
        RatingModel* model = models_[index.column() / COLUMNS];
        int column = index.column() % COLUMNS;
        if (index.row() < model->rowCount())
        {
            return model->index(index.row(), column).data(role);
        }
        else if (index.column() % COLUMNS == ENDED_COLUMN)
        {
            return Wt::WDate::currentServerDate();
        }
        else if (index.column() % COLUMNS == RATING_AFTER_COLUMN)
        {
            return "";
        }
    }

private:
    std::vector<RatingModel*> models_;
    int column_count_;
    int row_count_;
};

class RatingChangesImpl : public Wt::WContainerWidget
{
public:
    RatingChangesImpl(UserPtr user=UserPtr()):
    Wt::WContainerWidget(), number_of_users_(0)
    {
        model_ = new MultiRatingModel(this);
        chart_ = new Wt::Chart::WCartesianChart(Wt::Chart::ScatterPlot, this);
        chart_->setModel(model_);
        chart_->setXSeriesColumn(ENDED_COLUMN); // useless
        chart_->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateScale);
        chart_->resize(400, 200);
        if (!wApp->environment().ajax())
        {
            chart_->setPreferredMethod(Wt::WPaintedWidget::PngImage);
        }
        if (user)
        {
            add_user(user);
        }
    }

    void add_user(UserPtr user)
    {
        int shift = number_of_users_ * COLUMNS;
        int rating_after_column = RATING_AFTER_COLUMN + shift;
        int ended_column = ENDED_COLUMN + shift;
        model_->add_user(user);
        Wt::Chart::WDataSeries series(rating_after_column, Wt::Chart::CurveSeries);
        series.setXSeriesColumn(ended_column);
        series.setMarker(Wt::Chart::CircleMarker);
        chart_->addSeries(series);
        number_of_users_ += 1;
    }

private:
    int number_of_users_;
    MultiRatingModel* model_;
    Wt::Chart::WCartesianChart* chart_;
};

RatingChanges::RatingChanges(UserPtr user, Wt::WContainerWidget* parent):
Wt::WCompositeWidget(parent)
{
    impl_ = new RatingChangesImpl(user);
    setImplementation(impl_);
}

void RatingChanges::add_user(UserPtr user)
{
    impl_->add_user(user);
}

}
}

