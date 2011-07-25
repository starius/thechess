/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */


#include <Wt/WTableCell>
#include <Wt/WTableRow>
#include <Wt/WTableColumn>
#include <Wt/WText>
#include <Wt/WLabel>

#include "widgets/TableForm.hpp"

namespace thechess {
namespace widgets {

TableForm::TableForm(Wt::WContainerWidget* parent):
    Wt::WTable(parent) {
    setStyleClass("thechess_tableform");
}

void TableForm::section(const Wt::WString& header) {
    Wt::WTableCell* cell = elementAt(rowCount(), 0);
    cell->setColumnSpan(3);
    new Wt::WText(header, cell);
    cell->setStyleClass("thechess_tableform_header");
}

Wt::WContainerWidget* TableForm::item(const Wt::WString& name,
                                      const Wt::WString& description, Wt::WFormWidget* fw,
                                      Wt::WWidget* input, bool row) {
    int row_num = rowAt(rowCount())->rowNum();

    Wt::WTableCell* name_cell = elementAt(row_num, 0);
    Wt::WTableCell* input_cell = elementAt(row_num, 1);
    Wt::WTableCell* description_cell = elementAt(row_num, 2);
    if (!row) {
        input_cell = name_cell;
        description_cell = name_cell;
        name_cell->setColumnSpan(3);
    }
    if (row) {
        name_cell->setStyleClass("thechess_tableform_name");
        input_cell->setStyleClass("thechess_tableform_input");
        description_cell->setStyleClass("thechess_tableform_description");
    }

    Wt::WLabel* name_label = new Wt::WLabel(name, name_cell);
    name_label->setInline(false);
    if (!description.empty()) {
        Wt::WText* description_text = new Wt::WText(description, description_cell);
        description_text->setInline(false);
    }
    if (fw) {
        name_label->setBuddy(fw);
    }
    if (input) {
        input_cell->addWidget(input);
    }
    return input_cell;
}

void TableForm::show(Wt::WWidget* input) {
    parent_row_(input)->show();
}

void TableForm::hide(Wt::WWidget* input) {
    parent_row_(input)->hide();
}

Wt::WTableRow* TableForm::parent_row_(Wt::WWidget* input) {
    return rowAt(dynamic_cast<Wt::WTableCell*>(input->parent())->row());
}

}
}

