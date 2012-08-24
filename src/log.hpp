/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_LOG_HPP_
#define THECHESS_LOG_HPP_

#include <Wt/WString>
#include <Wt/Wc/global.hpp>

namespace thechess {

/** Log admin/moder action.
\warning tApp must be available.
*/
void admin_log(const Wt::WString& message);

/** Return HTML anchor */
Wt::WString html_a(Wt::Wc::url::IntegerNode* node, int id,
                   const Wt::WString& text);

/** Return HTML anchor to this user */
Wt::WString user_a(int user_id);

/** Return HTML anchor to this competition */
Wt::WString comp_a(int id);

/** Return HTML anchor to this comment */
Wt::WString comm_a(int id);

}

#endif

