/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_UTILS_TEXT_EDIT_HPP_
#define THECHESS_UTILS_TEXT_EDIT_HPP_

#include <Wt/WGlobal>

namespace thechess {

/** Call Wt::Wc::fix_text_edit(), add "emotions" and "image" buttons */
void patch_text_edit(Wt::WTextEdit* text_edit);

}

#endif

