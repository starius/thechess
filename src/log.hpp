/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_LOG_HPP_
#define THECHESS_LOG_HPP_

#include <Wt/WString>

namespace thechess {

/** Log admin/moder action.
\warning tApp must be available.
*/
void admin_log(const Wt::WString& message);

}

#endif
