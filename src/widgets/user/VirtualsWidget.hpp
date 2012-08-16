/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_USER_VIRTUALS_WIDGET_HPP_
#define THECHESS_WIDGETS_USER_VIRTUALS_WIDGET_HPP_

#include <Wt/WContainerWidget>

#include "model/all.hpp"

namespace thechess {

/** A widget, showing a graph of virtuals */
class VirtualsWidget : public Wt::WContainerWidget {
public:
    /** Constructor */
    VirtualsWidget(const dbo::Query<BD::BDPair>& pairs,
                   Wt::WContainerWidget* parent = 0);

    /** Constructor */
    VirtualsWidget(const UserPtr& user, Wt::WContainerWidget* parent = 0);

    /** Constructor */
    VirtualsWidget(Wt::WContainerWidget* parent = 0);

private:
    void initialize(const dbo::Query<BD::BDPair>& pairs);
};

}

#endif

