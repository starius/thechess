/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_USERLISTWIDGET_HPP_
#define THECHESS_WIDGETS_USERLISTWIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WContainerWidget>

namespace thechess {

class UserListModel;
class UserListView;

/** List of users */
class UserListWidget : public Wt::WContainerWidget {
public:
    /** Constructor */
    UserListWidget(Wt::WContainerWidget* parent = 0);

private:
    Wt::WCheckBox* oo_;
    Wt::WCheckBox* nr_;
    Wt::WCheckBox* b_;
    Wt::WLineEdit* name_;
    UserListModel* m_;
    UserListView* view_;

    void apply();
};

}

#endif

