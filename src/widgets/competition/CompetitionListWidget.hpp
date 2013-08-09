/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_
#define THECHESS_WIDGETS_COMPETITION_LIST_WIDGET_HPP_

#include <boost/tuple/tuple.hpp>

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

#include "model/global.hpp"
#include "widgets/global.hpp"

namespace thechess {

class CompetitionStateSelect;
class CompetitionListModel;
class CompetitionListView;

class CompetitionListWidget : public Wt::WContainerWidget {
public:
    CompetitionListWidget(Wt::WContainerWidget* parent = 0);

    CompetitionListWidget(const UserPtr& user, Wt::WContainerWidget* p = 0);

    CompetitionListWidget(const TeamPtr& team, Wt::WContainerWidget* p = 0);

private:
    CompetitionStateSelect* state_;
    CompetitionTypeWidget* type_;
    CompetitionListModel* model_;
    CompetitionListView* view_;
    Wt::WCheckBox* only_my_;
    Wt::WLineEdit* name_like_;
    UserPtr user_;
    TeamPtr team_;

    void initialize();
    void apply();
    void manager();
};

}

#endif

