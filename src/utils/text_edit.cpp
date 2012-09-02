/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WTextEdit>
#include <Wt/Wc/util.hpp>

#include "utils/text_edit.hpp"

namespace thechess {

static void add_emotions_and_image(Wt::WTextEdit* e) {
    const char* const buttons =
        "fontselect,|,bold,italic,underline,|"
        ",fontsizeselect,|,forecolor,backcolor,|"
        ",justifyleft,justifycenter,justifyright,justifyfull,|,anchor,image,|"
        ",numlist,bullist,|,emotions";
    e->setExtraPlugins("emotions");
    e->setConfigurationSetting("theme_advanced_buttons1", std::string(buttons));
}

void patch_text_edit(Wt::WTextEdit* text_edit) {
    add_emotions_and_image(text_edit);
    Wt::Wc::fix_text_edit(text_edit);
}

}

