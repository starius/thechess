/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_GP_LIST_WIDGET_HPP_
#define THECHESS_WIDGETS_GP_LIST_WIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>
#include <Wt/Dbo/Query>
#include <Wt/Dbo/QueryModel>

#include "model/all.hpp"

namespace thechess {

/** List model for query of GP */
class GPListModel;

/** View widget for query of GP */
class GPListView;

/** Select existing game parameters */
class GPListWidget : public Wt::WContainerWidget {
public:
    /** Result type for query */
    typedef GPPtr Result;

    /** Query type */
    typedef dbo::Query<Result> Q;

    /** Query model */
    typedef dbo::QueryModel<Result> BaseQM;

    /** Constructor */
    GPListWidget(Wt::WContainerWidget* parent = 0);

    /** Get selected gp */
    GPPtr gp() const;

    /** Set selected gp */
    void set_gp(const GPPtr& gp);

    /** Select first gp */
    void select_first();

private:
    GPListModel* model_;
    GPListView* view_;
    Wt::WCheckBox* only_my_;
    Wt::WLineEdit* name_like_;

    void apply();
    void manager();
};

}

#endif

