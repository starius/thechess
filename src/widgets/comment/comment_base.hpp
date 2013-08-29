/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_COMMENT_BASE_HPP_
#define THECHESS_WIDGETS_COMMENT_BASE_HPP_

#include <Wt/WGlobal>

#include "model/object.hpp"

namespace thechess {

/** Return object which is a base for this CHAT_ROOT.
If not found, then result.id == 0.
*/
Object comment_base(const CommentPtr& comment);

/** Return anchor text for global chat */
Wt::WString global_chat_text();

/** Return anchor link for global chat */
Wt::WLink global_chat_link();

/** Return anchor to global chat */
Wt::WAnchor* global_chat_anchor();

/** Return anchor text for game */
Wt::WString game_text(int id);

/** Return anchor link for game */
Wt::WLink game_link(int id);

/** Return anchor to game */
Wt::WAnchor* game_anchor(int id);

/** Return anchor text for competition */
Wt::WString competition_text(int id);

/** Return anchor link for competition */
Wt::WLink competition_link(int id);

/** Return anchor to competition */
Wt::WAnchor* competition_anchor(int id);

/** Return anchor text for team */
Wt::WString team_text(int id);

/** Return anchor link for team */
Wt::WLink team_link(int id);

/** Return anchor to team */
Wt::WAnchor* team_anchor(int id);

/** Return anchor text to page of base of this CHAT_ROOT */
Wt::WString host_text(const CommentPtr& comment);

/** Return anchor link to page of base of this CHAT_ROOT */
Wt::WLink host_link(const CommentPtr& comment);

/** Return anchor to page of base of this CHAT_ROOT.
If not found, then result == 0.
*/
Wt::WAnchor* anchor_to_host(const CommentPtr& comment);

}

#endif

