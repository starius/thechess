#ifndef THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_

#include <boost/tuple/tuple.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WCheckBox>
#include <Wt/Dbo/Query>
namespace dbo = Wt::Dbo;

#include "model/Competition.hpp"

namespace thechess {
namespace widgets {

class CompetitionListModel;
class CompetitionListView;

class CompetitionListWidget : public Wt::WContainerWidget
{
public:
    typedef boost::tuple<model::CompetitionPtr, Wt::WString, int> Result;
    typedef dbo::Query<Result> Q;

    CompetitionListWidget(Wt::WContainerWidget* parent=0);

private:
    CompetitionListModel* model_;
    Wt::WCheckBox* only_my_;

    Q query_();
    void apply_();
    void manager_();
};

}
}

#endif
