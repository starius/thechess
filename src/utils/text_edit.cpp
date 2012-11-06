/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/algorithm/string/replace.hpp>

#include <Wt/WApplication>
#include <Wt/WTextEdit>
#include <Wt/Wc/util.hpp>

#include "utils/text_edit.hpp"
#include "model/all.hpp"

namespace thechess {

static void add_emotions_and_image(Wt::WTextEdit* e) {
    const char* const b1 =
        "fontselect,|,bold,italic,underline,|"
        ",fontsizeselect,|,forecolor,backcolor,|"
        ",justifyleft,justifycenter,justifyright,justifyfull,|"
        ",link,image,|"
        ",numlist,bullist,|,emotions,|,blockquote";
    e->setConfigurationSetting("theme_advanced_buttons1", std::string(b1));
    if (User::has_s(SWITCH_MORE_FORMATTING)) {
        const char* const b2 =
            "insertdate,inserttime,|,tablecontrols,|,"
            "hr,removeformat,|,formatselect,sub,sup,strikethrough,styleprops,|,"
            "fullscreen";
        e->setConfigurationSetting("theme_advanced_buttons2", std::string(b2));
        e->setExtraPlugins("advimage,emotions,inlinepopups,insertdatetime,"
                           "table,fullscreen,style");
    } else {
        e->setExtraPlugins("advimage,emotions,inlinepopups");
    }
    e->setConfigurationSetting("language", wApp->locale());
    e->setConfigurationSetting("force_br_newlines", true);
    e->setConfigurationSetting("force_p_newlines", false);
    e->setConfigurationSetting("forced_root_block", std::string());
    e->setConfigurationSetting("convert_urls", false);
    e->setConfigurationSetting("content_css",
                               std::string("/css/mycontent.css"));
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

