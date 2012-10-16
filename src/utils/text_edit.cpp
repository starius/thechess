/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/replace.hpp>

#include <Wt/WTextEdit>
#include <Wt/Wc/util.hpp>

#include "utils/text_edit.hpp"

namespace thechess {

static void add_emotions_and_image(Wt::WTextEdit* e) {
    const char* const buttons =
        "fontselect,|,bold,italic,underline,|"
        ",fontsizeselect,|,forecolor,backcolor,|"
        ",justifyleft,justifycenter,justifyright,justifyfull,|,link,image,|"
        ",insertdate,inserttime,|"
        ",numlist,bullist,|,emotions";
    e->setExtraPlugins("emotions,inlinepopups,insertdatetime");
    e->setConfigurationSetting("theme_advanced_buttons1", std::string(buttons));
    e->setConfigurationSetting("force_br_newlines", true);
    e->setConfigurationSetting("force_p_newlines", false);
    e->setConfigurationSetting("forced_root_block", std::string());
    e->setConfigurationSetting("convert_urls", false);
}

void patch_text_edit(Wt::WTextEdit* text_edit) {
    add_emotions_and_image(text_edit);
    text_edit->resize(600, 300);
    text_edit->setRows(20);
    text_edit->setColumns(80);
    Wt::Wc::fix_text_edit(text_edit);
}

Wt::WString patch_text_edit_text(const Wt::WString& text) {
    std::string value = text.toUTF8();
    boost::replace_all(value, "src=\"../../../../../", "src=\"/");
    boost::replace_all(value, "src=\"../../../../", "src=\"/");
    boost::replace_all(value, "src=\"../../../", "src=\"/");
    boost::replace_all(value, "src=\"../../", "src=\"/");
    boost::replace_all(value, "src=\"../", "src=\"/");
    return Wt::WString::fromUTF8(value);
}

}

