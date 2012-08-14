/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_USER_VIRTUALS_WIDGET_HPP_
#define THECHESS_WIDGETS_USER_VIRTUALS_WIDGET_HPP_

#include <Wt/Wc/FilterResource.hpp>

#include "model/all.hpp"

namespace thechess {

/** A widget, showing a graph of virtuals */
class VirtualsWidget : public Wt::Wc::FilterResource {
public:
    /** Constructor */
    VirtualsWidget(const dbo::Query<BD::BDPair>& pairs,
                   Wt::WObject* parent = 0);

    /** Constructor */
    VirtualsWidget(const UserPtr& user, Wt::WObject* parent = 0);

    /** Constructor */
    VirtualsWidget(Wt::WObject* parent = 0);

    /** Output competition tree in DOT format (graphviz) */
    void write_input(std::ostream& out) const;

private:
    dbo::Query<BD::BDPair> pairs_;
};

}

#endif

