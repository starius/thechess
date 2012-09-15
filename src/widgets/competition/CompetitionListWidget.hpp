/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_

#include <boost/tuple/tuple.hpp>

#include <Wt/WContainerWidget>
#include <Wt/WCheckBox>

#include "model/global.hpp"

namespace thechess {

class CompetitionStateSelect;
class CompetitionListModel;
class CompetitionListView;

class CompetitionListWidget : public Wt::WContainerWidget {
public:
    CompetitionListWidget(Wt::WContainerWidget* parent = 0);

    CompetitionListWidget(const UserPtr& user, Wt::WContainerWidget* p = 0);

private:
    CompetitionStateSelect* state_;
    CompetitionListModel* model_;
    Wt::WCheckBox* only_my_;
    UserPtr user_;

    void initialize();
    void apply();
    void manager();
};

}

#endif

