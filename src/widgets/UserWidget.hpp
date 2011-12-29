/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_USEREWIDGET_HPP_
#define THECHESS_WIDGETS_USEREWIDGET_HPP_

#include <Wt/WGlobal>
#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>

#include "model/all.hpp"

namespace thechess {

class UserWidgetImpl;

class UserWidget : public Wt::WCompositeWidget {
public:
    UserWidget(const UserPtr& user, Wt::WContainerWidget* parent = 0);
private:
    UserWidgetImpl* impl_;
};

}

#endif

