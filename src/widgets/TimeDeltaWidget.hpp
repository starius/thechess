/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_TIMEDELTA_HPP_
#define THECHESS_WIDGETS_TIMEDELTA_HPP_

#include <Wt/WCompositeWidget>
#include <Wt/Dbo/ptr>

#include "utils/time_intervals.hpp"

namespace Wt {
class WContainerWidget;
class WFormWidget;
}

namespace thechess {

class TimeDeltaWidgetImpl;

class TimeDeltaWidget : public Wt::WCompositeWidget {
public:
    TimeDeltaWidget(const Td& min, const Td& value, const Td& max,
                    Wt::WContainerWidget* parent = 0);
    Td value() const;
    Wt::WFormWidget* form_widget(); // for labbels

private:
    TimeDeltaWidgetImpl* impl_;
};

}

#endif

