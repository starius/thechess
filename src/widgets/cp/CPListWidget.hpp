/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_CP_LIST_WIDGET_HPP_
#define THECHESS_WIDGETS_CP_LIST_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>

#include "model/all.hpp"
#include "widgets/global.hpp"

namespace thechess {

/** List model for query of CP */
class CPListModel;

/** View widget for query of CP */
class CPListView;

/** Select existing game parameters */
class CPListWidget : public Wt::WContainerWidget {
public:
    /** Result type for query */
    typedef CPPtr Result;

    /** Query type */
    typedef dbo::Query<Result> Q;

    /** Query model */
    typedef dbo::QueryModel<Result> BaseQM;

    /** Constructor */
    CPListWidget(Wt::WContainerWidget* parent = 0);

    /** Get selected cp */
    CPPtr cp() const;

    /** Set selected cp */
    void set_cp(const CPPtr& cp);

private:
    CPListModel* model_;
    CPListView* view_;
    Wt::WCheckBox* only_my_;
    CompetitionTypeWidget* type_;
    Wt::WLineEdit* name_like_;

    void apply();
    void manager();
    void select_first();
};

}

#endif

