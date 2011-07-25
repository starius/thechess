/*
 * thechess, chess game web application written in C++ and based on Wt
 * Copyright (C) 2010 Boris Nagaev
 *
 * thechess is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef THECHESS_WIDGETS_TABLE_FORM_HPP_
#define THECHESS_WIDGETS_TABLE_FORM_HPP_

#include <Wt/WContainerWidget>
#include <Wt/WTable>
#include <Wt/WTableRow>
#include <Wt/WString>
#include <Wt/WWidget>
#include <Wt/WFormWidget>

namespace thechess {
namespace widgets {

class TableForm : public Wt::WTable {
public:
    TableForm(Wt::WContainerWidget* parent=0);
    void section(const Wt::WString& header);
    Wt::WContainerWidget* item(const Wt::WString& name,
                               const Wt::WString& description="",
                               Wt::WFormWidget* fw=0, Wt::WWidget* input=0, bool row=true);
    void show(Wt::WWidget* input);
    void hide(Wt::WWidget* input);

private:
    Wt::WTableRow* parent_row_(Wt::WWidget* input);
};

}
}


#endif

