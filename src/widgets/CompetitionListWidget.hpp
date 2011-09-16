/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_

#include <boost/tuple/tuple.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WCheckBox>
#include <Wt/Dbo/Query>
namespace dbo = Wt::Dbo;

#include "model/all.hpp"

namespace thechess {

class CompetitionListModel;
class CompetitionListView;

class CompetitionListWidget : public Wt::WContainerWidget {
public:
    CompetitionListWidget(Wt::WContainerWidget* parent = 0);

private:
    CompetitionListModel* model_;
    Wt::WCheckBox* only_my_;

    void apply_();
    void manager_();
};

}

#endif

