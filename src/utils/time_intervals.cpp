/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include "Application.hpp"

namespace thechess {

static Wt::WDateTime corrected_time(const Wt::WDateTime& time) {
    if (tApp) {
        return time + tApp->timezone_diff();
    } else {
        return time;
    }
}

Wt::WString time2str(const Wt::WDateTime& time, const Wt::WString& format) {
    return corrected_time(time).toString(format);
}

}

