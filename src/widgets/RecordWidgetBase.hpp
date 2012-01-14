/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_RECORD_WIDGET_BASE_HPP_
#define THECHESS_WIDGETS_RECORD_WIDGET_BASE_HPP_

#include <Wt/WGlobal>
#include <Wt/Wc/global.hpp>

#include "model/record.hpp"

namespace thechess {

/** Input fields for Record */
struct RecordWidgetBase {
    /** Name */
    Wt::WLineEdit* name;

    /** Description */
    Wt::WTextEdit* description;

    /** Create input widgets.
    \param record A record to read from (0 = empty record).
    \param form   A form to add widgets to.
    */
    void add_record_inputs(const Record* record, Wt::Wc::TableForm* form);

    /** Write info.
    This sets name and description.
    If \c init argument is true, init user is set to tApp->user().
    */
    void write_record(Record* record, bool init = true) const;
};

}

#endif

