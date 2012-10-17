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
draft=true is for actions of non-admins or for read-only actions of admins.

user is the source of the message.
*/
void admin_log(const Wt::WString& message, UserPtr user, bool draft = false);

/** Log admin/moder action.
\warning tApp must be available.

Overloaded function. tApp->user() is used.
*/
void admin_log(const Wt::WString& message, bool draft = false);

/** Return HTML anchor */
Wt::WString html_a(const Wt::WString& path, const Wt::WString& text);

/** Return HTML anchor */
Wt::WString html_a(Wt::Wc::url::IntegerNode* node, int id,
                   const Wt::WString& text);

/** Return HTML anchor to this user */
Wt::WString user_a(int user_id);

/** Return HTML anchor to this competition */
Wt::WString comp_a(int id);

/** Return HTML anchor to this comment */
Wt::WString comm_a(int id);

/** Return HTML anchor to this game */
Wt::WString game_a(int id);

/** Return HTML anchor for this IP */
Wt::WString ip_a(const std::string& ip);

}

#endif

