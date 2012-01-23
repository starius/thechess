/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <utility>
#include <vector>
#include <boost/foreach.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WString>
#include <Wt/Dbo/Transaction>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>

#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
#include <Wt/WDate>
#include <Wt/WEnvironment>
#include <Wt/WApplication>

#include "widgets/user/RatingChanges.hpp"
#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

namespace RP {
typedef GamePtr Result;
typedef dbo::Query<Result> Q;
typedef dbo::QueryModel<Result> BaseQM;

}

class RatingModel : public RP::BaseQM {
public:
    static const int ID_COLUMN = 0;
    static const int ENDED_COLUMN = 1;
    static const int RATING_AFTER_COLUMN = 2;
    static const int COLUMNS = 3;

    RatingModel(const UserPtr& user, Wt::WObject* parent = 0) :
        RP::BaseQM(parent), user_(user) {
        dbo::Transaction t(tApp->session());
        RP::Q query = tApp->session().find<Game>();
        long long uid = user_.id(); // FIXME http://redmine.emweb.be/issues/1124
        query.where("white_id = ? or black_id = ?").bind(uid).bind(uid);
        query.where("rating_after_white != -1");
        query.orderBy("ended");
        setQuery(query);
        addColumn("id", Wt::WString::tr("tc.common.number"));
        addColumn("ended", Wt::WString::tr("tc.game.ended"));
        addColumn("rating_after_white",
                  Wt::WString::tr("tc.user.rating_after"));
        t.commit();
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        if (index.column() == ENDED_COLUMN && role == Wt::DisplayRole) {
            const GamePtr& game = resultRow(index.row());
            return Wt::WDate(game->ended().date());
        }
        if (index.column() == RATING_AFTER_COLUMN && role == Wt::DisplayRole) {
            const GamePtr& game = resultRow(index.row());
            return game->rating_after(game->color_of(user_));
        } else {
            return RP::BaseQM::data(index, role);
        }
    }

private:
    const UserPtr user_;
};

class MultiRatingModel : public Wt::WAbstractTableModel {
public:
    MultiRatingModel(Wt::WObject* parent = 0):
        Wt::WAbstractTableModel(parent), column_count_(0), row_count_(0) {
    }

    ~MultiRatingModel() {
        BOOST_FOREACH (RatingModel* model, models_) {
            delete model;
        }
        models_.clear();
    }

    void add_user(const UserPtr& user) {
        RatingModel* model = new RatingModel(user, this);
        models_.push_back(model);
        column_count_ += RatingModel::COLUMNS;
        row_count_ = std::max(row_count_, model->rowCount());
    }

    virtual int columnCount(const Wt::WModelIndex& /* parent */) const {
        return column_count_;
    }

    virtual int rowCount(const Wt::WModelIndex& /* parent */) const {
        return row_count_;
    }

    boost::any data(const Wt::WModelIndex& index,
                    int role = Wt::DisplayRole) const {
        RatingModel* model = models_[index.column() / RatingModel::COLUMNS];
        int column = index.column() % RatingModel::COLUMNS;
        if (index.row() < model->rowCount()) {
            return model->index(index.row(), column).data(role);
        } else if (index.column() % RatingModel::COLUMNS ==
                   RatingModel::ENDED_COLUMN) {
            return Wt::WDate::currentServerDate();
        }
        return "";
    }

private:
    std::vector<RatingModel*> models_;
    int column_count_;
    int row_count_;
};

class RatingChanges::RatingChangesImpl : public Wt::WContainerWidget {
public:
    RatingChangesImpl(const UserPtr& user = UserPtr()):
        Wt::WContainerWidget(), number_of_users_(0) {
        model_ = new MultiRatingModel(this);
        chart_ = new Wt::Chart::WCartesianChart(Wt::Chart::ScatterPlot, this);
        chart_->setModel(model_);
        chart_->setXSeriesColumn(RatingModel::ENDED_COLUMN); // useless
        chart_->axis(Wt::Chart::XAxis).setScale(Wt::Chart::DateScale);
        chart_->resize(400, 200);
        if (!wApp->environment().ajax()) {
            chart_->setPreferredMethod(Wt::WPaintedWidget::PngImage);
        }
        if (user) {
            add_user(user);
        }
    }

    void add_user(const UserPtr& user) {
        int shift = number_of_users_ * RatingModel::COLUMNS;
        int rating_after_column = RatingModel::RATING_AFTER_COLUMN + shift;
        int ended_column = RatingModel::ENDED_COLUMN + shift;
        model_->add_user(user);
        Wt::Chart::WDataSeries series(rating_after_column,
                                      Wt::Chart::CurveSeries);
        chart_->addSeries(series);
        series.setXSeriesColumn(ended_column);
        series.setMarker(Wt::Chart::CircleMarker);
        number_of_users_ += 1;
    }

private:
    int number_of_users_;
    MultiRatingModel* model_;
    Wt::Chart::WCartesianChart* chart_;
};

RatingChanges::RatingChanges(const UserPtr& user, Wt::WContainerWidget* parent):
    Wt::WCompositeWidget(parent) {
    impl_ = new RatingChangesImpl(user);
    setImplementation(impl_);
}

void RatingChanges::add_user(const UserPtr& user) {
    impl_->add_user(user);
}

}

