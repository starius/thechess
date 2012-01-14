/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WLineEdit>
#include <Wt/WTextEdit>
#include <Wt/Wc/TableForm.hpp>

#include "widgets/RecordWidgetBase.hpp"
#include "widgets/user/ClassificationWidget.hpp"
#include "config.hpp"
#include "model/all.hpp"
#include "Application.hpp"

namespace thechess {

void RecordWidgetBase::add_record_inputs(const Record* record,
        Wt::Wc::TableForm* form) {
    Record r;
    if (record == 0) {
        record = &r;
    }
    name = new Wt::WLineEdit(record->name());
    name->setMaxLength(config::MAX_NAME);
    form->item(Wt::WString::tr("tc.common.Name"), "", name, name);
    description = new Wt::WTextEdit(record->description());
    form->item(Wt::WString::tr("tc.common.Description"), "",
               description, description);
}

void RecordWidgetBase::write_record(Record* record, bool init) const {
    std::wstring name_str = name->text().value();
    if (name_str.length() > config::MAX_NAME) {
        name_str.resize(config::MAX_NAME);
    }
    record->set_name(name_str);
    std::wstring description_str = description->text().value();
    if (description_str.length() > config::MAX_DESCRIPTION) {
        description_str.resize(config::MAX_DESCRIPTION);
    }
    record->set_description(description_str);
    if (init) {
        record->set_init(tApp->user());
    }
}

}

